// SPDX-License-Identifier: GPL-2.0+ OR BSD-3-Clause
/*
 * Copyright (C) 2018, STMicroelectronics - All Rights Reserved
 */
#include <common.h>
#include <adc.h>
#include <bootm.h>
#include <clk.h>
#include <config.h>
#include <dm.h>
#include <env.h>
#include <env_internal.h>
#include <fdt_simplefb.h>
#include <fdt_support.h>
#include <g_dnl.h>
#include <generic-phy.h>
#include <hang.h>
#include <i2c.h>
#include <regmap.h>
#include <init.h>
#include <log.h>
#include <malloc.h>
#include <misc.h>
#include <mtd_node.h>
#include <net.h>
#include <netdev.h>
#include <phy.h>
#include <remoteproc.h>
#include <reset.h>
#include <syscon.h>
#include <usb.h>
#include <watchdog.h>
#include <asm/io.h>
#include <asm/gpio.h>
#include <asm/arch/stm32.h>
#include <asm/arch/sys_proto.h>
#include <jffs2/load_kernel.h>
#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/iopoll.h>
#include <power/regulator.h>
#include <usb/dwc2_udc.h>
#include <spi.h>

#include "../../st/common/stusb160x.h"

/* SYSCFG registers */
#define SYSCFG_BOOTR		0x00
#define SYSCFG_PMCSETR		0x04
#define SYSCFG_IOCTRLSETR	0x18
#define SYSCFG_ICNR		0x1C
#define SYSCFG_CMPCR		0x20
#define SYSCFG_CMPENSETR	0x24
#define SYSCFG_PMCCLRR		0x44

#define SYSCFG_BOOTR_BOOT_MASK		GENMASK(2, 0)
#define SYSCFG_BOOTR_BOOTPD_SHIFT	4

#define SYSCFG_IOCTRLSETR_HSLVEN_TRACE		BIT(0)
#define SYSCFG_IOCTRLSETR_HSLVEN_QUADSPI	BIT(1)
#define SYSCFG_IOCTRLSETR_HSLVEN_ETH		BIT(2)
#define SYSCFG_IOCTRLSETR_HSLVEN_SDMMC		BIT(3)
#define SYSCFG_IOCTRLSETR_HSLVEN_SPI		BIT(4)

#define SYSCFG_CMPCR_SW_CTRL		BIT(1)
#define SYSCFG_CMPCR_READY		BIT(8)

#define SYSCFG_CMPENSETR_MPU_EN		BIT(0)

/*
 * Get a global data pointer
 */
DECLARE_GLOBAL_DATA_PTR;

#define USB_LOW_THRESHOLD_UV		200000
#define USB_WARNING_LOW_THRESHOLD_UV	660000
#define USB_START_LOW_THRESHOLD_UV	1230000
#define USB_START_HIGH_THRESHOLD_UV	2150000

/* Vendor specific initialization command sequence for GC9503V IC */

const unsigned char spi5_bootpic_init[] =
{
       1, 0x01, // Software Reset
       3, 0x00, // Long delay

       // Display off
       1, 0x28,

       // Sleep In & Booster off
       1, 0x10,
       3, 0x00, // Long delay

       // EXTC Command 'Set page 0 enable register'
       1, 0xF0, 0, 0x55, 0, 0xAA, 0, 0x52, 0, 0x08, 0, 0x00, // Unlock magic.

       // OTP_CTL
       1, 0xF6, 0, 0x5A, 0, 0x87,

       //
       1, 0xC1, 0, 0x3F,

       //
       1, 0xC2, 0, 0x0E,

       //
       1, 0xC6, 0, 0xF8,

       //
       1, 0xC9, 0, 0x10,

       //
       1, 0xCD, 0, 0x25,

       //
       1, 0xF8, 0, 0x8A,

       //
       1, 0xAC, 0, 0x65,

       //
       1, 0xA7, 0, 0x47,

       // CHP_CTL5
       1, 0xA0, 0, 0x88,

       //
       1, 0xFA, 0, 0x02, 0, 0x02, 0, 0x02,

       //
       1, 0xA3, 0, 0x2E,

       //
       1, 0xFD, 0, 0x28, 0, 0x3C, 0, 0x00,

       //
       1, 0x71, 0, 0x48,

       //
       1, 0x72, 0, 0x48,

       //
       1, 0x73, 0, 0x00, 0, 0x44,

       //
       1, 0x97, 0, 0xEE,

       //
       1, 0x83, 0, 0x93,

       // VREG_CTL4
       1, 0x9A, 0, 0xB5,

       // VREG_CTL5
       1, 0x9B, 0, 0x2C,

       // VREG_CTL1
       1, 0x82, 0, 0x30, 0, 0x30,

       // DISPLAY_CTL
       1, 0xB1, 0, 0x10,

       //
       1, 0x7A, 0, 0x13, 0, 0x1A,

       //
       1, 0x7B, 0, 0x13, 0, 0x1A,

       // CHP_CTL1
       1, 0x86, 0, 0x99, 0, 0xA3, 0, 0xA3, 0, 0x61,

       //
       1, 0x6D, 0, 0x00, 0, 0x1D, 0, 0x0A, 0, 0x10, 0, 0x08, 0, 0x1F, 0, 0x1E, 0, 0x1E, 0, 0x1E, 0, 0x1E, 0, 0x1E, 0, 0x1E, 0, 0x1E, 0, 0x1E, 0, 0x1E, 0, 0x1E, \
                0, 0x1E, 0, 0x1E, 0, 0x1E, 0, 0x1E, 0, 0x1E, 0, 0x1E, 0, 0x1E, 0, 0x1E, 0, 0x1E, 0, 0x1E, 0, 0x1F, 0, 0x01, 0, 0x0F, 0, 0x09, 0, 0x1D, 0, 0x00,

       //
       1, 0x64, 0, 0x18, 0, 0x04, 0, 0x03, 0, 0xC7, 0, 0x03, 0, 0x03, 0, 0x18, 0, 0x03, 0, 0x03, 0, 0xC6, 0, 0x03, 0, 0x03, 0, 0x0B, 0, 0x7A, 0, 0x0B, 0, 0x7A,

       //
       1, 0x67, 0, 0x18, 0, 0x02, 0, 0x03, 0, 0xC5, 0, 0x03, 0, 0x03, 0, 0x18, 0, 0x01, 0, 0x03, 0, 0xC4, 0, 0x03, 0, 0x03, 0, 0x0B, 0, 0x7A, 0, 0x0B, 0, 0x7A,

       //
       1, 0x60, 0, 0x18, 0, 0x06, 0, 0x0B, 0, 0x7A, 0, 0x18, 0, 0x02, 0, 0x0B, 0, 0x7A,

       //
       1, 0x63, 0, 0x18, 0, 0x02, 0, 0x0B, 0, 0x7A, 0, 0x18, 0, 0x05, 0, 0x0B, 0, 0x7A,

       //
       1, 0x69, 0, 0x14, 0, 0x22, 0, 0x14, 0, 0x22, 0, 0x44, 0, 0x22, 0, 0x08,

       //
       1, 0xD1, 0, 0x00, 0, 0x00, 0, 0x00, 0, 0x10, 0, 0x00, 0, 0x46, 0, 0x00, 0, 0x5F, 0, 0x00, 0, 0x7B, 0, 0x00, 0, 0xA5, 0, 0x00, 0, 0xC8, 0, 0x00, 0, 0xFF, 0, 0x01, \
                0, 0x29, 0, 0x01, 0, 0x6E, 0, 0x01, 0, 0xA7, 0, 0x01, 0, 0xFA, 0, 0x02, 0, 0x3D, 0, 0x02, 0, 0x3F, 0, 0x02, 0, 0x7D, 0, 0x02, 0, 0xCA, 0, 0x03, 0, 0x00, \
                0, 0x03, 0, 0x4A, 0, 0x03, 0, 0x85, 0, 0x03, 0, 0xA9, 0, 0x03, 0, 0xC8, 0, 0x03, 0, 0xE8, 0, 0x03, 0, 0xF0, 0, 0x03, 0, 0xF5, 0, 0x03, 0, 0xFE, 0, 0x03, 0, 0xFF,

       //
       1, 0xD2, 0, 0x00, 0, 0x00, 0, 0x00, 0, 0x10, 0, 0x00, 0, 0x46, 0, 0x00, 0, 0x5F, 0, 0x00, 0, 0x7B, 0, 0x00, 0, 0xA5, 0, 0x00, 0, 0xC8, 0, 0x00, 0, 0xFF, 0, 0x01, \
                0, 0x29, 0, 0x01, 0, 0x6E, 0, 0x01, 0, 0xA7, 0, 0x01, 0, 0xFA, 0, 0x02, 0, 0x3D, 0, 0x02, 0, 0x3F, 0, 0x02, 0, 0x7D, 0, 0x02, 0, 0xCA, 0, 0x03, 0, 0x00, \
                0, 0x03, 0, 0x4A, 0, 0x03, 0, 0x85, 0, 0x03, 0, 0xA9, 0, 0x03, 0, 0xC8, 0, 0x03, 0, 0xE8, 0, 0x03, 0, 0xF0, 0, 0x03, 0, 0xF5, 0, 0x03, 0, 0xFE, 0, 0x03, 0, 0xFF,

       //
       1, 0xD3, 0, 0x00, 0, 0x00, 0, 0x00, 0, 0x10, 0, 0x00, 0, 0x46, 0, 0x00, 0, 0x5F, 0, 0x00, 0, 0x7B, 0, 0x00, 0, 0xA5, 0, 0x00, 0, 0xC8, 0, 0x00, 0, 0xFF, 0, 0x01, \
                0, 0x29, 0, 0x01, 0, 0x6E, 0, 0x01, 0, 0xA7, 0, 0x01, 0, 0xFa, 0, 0x02, 0, 0x3D, 0, 0x02, 0, 0x3F, 0, 0x02, 0, 0x7D, 0, 0x02, 0, 0xCA, 0, 0x03, 0, 0x00, \
                0, 0x03, 0, 0x4A, 0, 0x03, 0, 0x85, 0, 0x03, 0, 0xA9, 0, 0x03, 0, 0xC8, 0, 0x03, 0, 0xE8, 0, 0x03, 0, 0xF0, 0, 0x03, 0, 0xF5, 0, 0x03, 0, 0xFE, 0, 0x03, 0, 0xFF,

       //
       1, 0xD4, 0, 0x00, 0, 0x00, 0, 0x00, 0, 0x10, 0, 0x00, 0, 0x46, 0, 0x00, 0, 0x5F, 0, 0x00, 0, 0x7B, 0, 0x00, 0, 0xA5, 0, 0x00, 0, 0xC8, 0, 0x00, 0, 0xFF, 0, 0x01, \
                0, 0x29, 0, 0x01, 0, 0x6E, 0, 0x01, 0, 0xA7, 0, 0x01, 0, 0xFA, 0, 0x02, 0, 0x3D, 0, 0x02, 0, 0x3F, 0, 0x02, 0, 0x7D, 0, 0x02, 0, 0xCA, 0, 0x03, 0, 0x00, \
                0, 0x03, 0, 0x4A, 0, 0x03, 0, 0x85, 0, 0x03, 0, 0xA9, 0, 0x03, 0, 0xC8, 0, 0x03, 0, 0xE8, 0, 0x03, 0, 0xF0, 0, 0x03, 0, 0xF5, 0, 0x03, 0, 0xFE, 0, 0x03, 0, 0xFF,

       //
       1, 0xD5, 0, 0x00, 0, 0x00, 0, 0x00, 0, 0x10, 0, 0x00, 0, 0x46, 0, 0x00, 0, 0x5F, 0, 0x00, 0, 0x7B, 0, 0x00, 0, 0xA5, 0, 0x00, 0, 0xC8, 0, 0x00, 0, 0xFF, 0, 0x01, \
                0, 0x29, 0, 0x01, 0, 0x6E, 0, 0x01, 0, 0xA7, 0, 0x01, 0, 0xFA, 0, 0x02, 0, 0x3D, 0, 0x02, 0, 0x3F, 0, 0x02, 0, 0x7D, 0, 0x02, 0, 0xCA, 0, 0x03, 0, 0x00, \
                0, 0x03, 0, 0x4A, 0, 0x03, 0, 0x85, 0, 0x03, 0, 0xA9, 0, 0x03, 0, 0xC8, 0, 0x03, 0, 0xE8, 0, 0x03, 0, 0xF0, 0, 0x03, 0, 0xF5, 0, 0x03, 0, 0xFE, 0, 0x03, 0, 0xFF,

       //
       1, 0xD6, 0, 0x00, 0, 0x00, 0, 0x00, 0, 0x10, 0, 0x00, 0, 0x46, 0, 0x00, 0, 0x5F, 0, 0x00, 0, 0x7B, 0, 0x00, 0, 0xA5, 0, 0x00, 0, 0xC8, 0, 0x00, 0, 0xFF, 0, 0x01, \
                0, 0x29, 0, 0x01, 0, 0x6E, 0, 0x01, 0, 0xA7, 0, 0x01, 0, 0xFA, 0, 0x02, 0, 0x3D, 0, 0x02, 0, 0x3F, 0, 0x02, 0, 0x7D, 0, 0x02, 0, 0xCA, 0, 0x03, 0, 0x00, \
                0, 0x03, 0, 0x4A, 0, 0x03, 0, 0x85, 0, 0x03, 0, 0xA9, 0, 0x03, 0, 0xC8, 0, 0x03, 0, 0xE8, 0, 0x03, 0, 0xF0, 0, 0x03, 0, 0xF5, 0, 0x03, 0, 0xFE, 0, 0x03, 0, 0xFF,

       // Interface Pixel Format
       1, 0x3A, 0, 0x77,

       // RGB Interface Signals Control
       1, 0xB0, 0, 0x0B, 0, 0x08, 0, 0x10, 0, 0x1E, 0, 0x1E,

       // Sleep Out & Booster on
       1, 0x11,

       // Long delay
       3, 0x00,

       // Display on
       1, 0x29,
};

void init_gc9503v_display_cntrl(void)
{
    int i, k;
    int len, sze;
    const unsigned char* pb;
    unsigned char* ps;
    struct spi_slave *slave;
    struct udevice *dev;
    char name[30], *str;
    unsigned char txbuf[400];
    unsigned char rxbuf[400];
    
    printf("\nInitialize GC9503V display controller via SPI.\n");

    /* Setup GPIO. */
	
    if (gpio_request(94 /*PF14*/, "WR_DISPLAY") == -1)
    {
        printf("init_gc9503v_display_cntrl() - GPIO request failed for PF14 (94)\n");  
    }
    else
    {
	gpio_direction_output(94 /*PF14 WR_DISPLAY*/, 0);

        /* Setup SPI. */

	slave = 0;
	snprintf(name, sizeof(name), "generic_%d:%d", 5 /*SPI5*/, 0 /*NPCS0*/);
	str = strdup(name);

	if (str)
	{
		if (spi_get_bus_and_cs(5 /*SPI5*/, 0 /*NPCS0*/, 32000000, SPI_MODE_0/*SPI_CS_HIGH*/, 
		                      "spi_generic_drv", str, &dev, &slave))
		{
			slave = 0;
		}
	}
	
        if (slave)
        {
            /* Chip select */
			
            if (spi_claim_bus(slave))
            {
                printf("init_gc9503v_display_cntrl() - spi_claim_bus() failed\n");  
            }
            else
            {
                mdelay(100);

                /* Transmit initialization sequence. */

                pb  = spi5_bootpic_init;
                len = (sizeof(spi5_bootpic_init)) >> 1;
                sze = 0;

                for (i = 0; i < len; i++)
                {
                    switch (*pb)
                    {
                        case 3: // Delay long
                            mdelay(100);  // 100 ms
                        break;
						
                        case 2: // Delay short
                            mdelay(10);  // 10 ms
                        break;

                        case 1: // Command
                            // Set "Command/Data" pin to low.

                            gpio_direction_output(94 /*PF14 WR_DISPLAY*/, 0);
                            udelay(20);
                            spi_xfer(slave, 8, pb + 1, rxbuf, SPI_XFER_BEGIN | SPI_XFER_END);
                        break;

                        case 4: // Read Data
                            sze = *(pb + 1);
                            ps  = rxbuf;

                            memset(ps, 0, sze);

                            spi_xfer(slave, sze << 3, txbuf, rxbuf, SPI_XFER_BEGIN | SPI_XFER_END);
                        break;

                        case 0: // Send Data
                            ps  = txbuf;
                            sze = 0;

                            do
                            {
                                /* Copy data. */

                                *ps++ = *(pb + 1);

                                sze++;

                                pb += 2;

                                /* Check table len. */

                                if (++i >= len)
                                {
                                    break;
                                }
                            }
                            while (!(*pb) /* Still data bytes? */);

                            /* Mind the increment at the end of the loop. */

                            pb -= 2;

                            i--;

                            // Set "Command/Data" pin to high.

                            gpio_direction_output(94 /*PF14 WR_DISPLAY*/, 1);
                            udelay(20);
                            spi_xfer(slave, sze << 3, txbuf, rxbuf, SPI_XFER_BEGIN | SPI_XFER_END);
                            break;

                        default:
                            break;
                    }

                    /* Short delay after the last command. */

                    mdelay(10);

                    /* Next command. */

                    pb += 2;
                }

                mdelay(50);

                /* Release the SPI. */

                spi_release_bus(slave);
            }

            spi_free_slave(slave);
        }
        
        gpio_direction_output(94 /*PF14 WR_DISPLAY*/, 0);
        gpio_free(94 /*PF14*/);
    }
}

void init_board_gpios_ios(void)
{
    /* Setup the background light of the display and turn it on. */
    
    if (gpio_request(83 /*PF3 Back light*/, "BACK_LIGHT") != -1)
    {
        gpio_direction_output(83 /*PF3 Back light*/, 1);
    }
	
    /* Setup GPIOs used later on for the shift registers. */

    if (gpio_request(27 /*PB11*/, "SR_DATA_IN") == -1)
    {
        printf("init_board_gpios_ios() - GPIO request failed for PB11 (27)\n");
    }
    else
    {
        gpio_direction_output(27 /*PB11*/, 0);
        gpio_free(27 /*PB11*/);
    }

    if (gpio_request(13 /*PA13*/, "SR_CLK") == -1)
    {
        printf("init_board_gpios_ios() - GPIO request failed for PA13 (13)\n");
    }
    else
    {
        gpio_direction_output(13 /*PA13*/, 0);
        gpio_free(13 /*PA13*/);
    }

    if (gpio_request(3 /*PA3*/, "SR_EN_OUT") == -1)
    {
        printf("init_board_gpios_ios() - GPIO request failed for PA3 (3)\n");
    }
    else
    {
        gpio_direction_output(3 /*PA3*/, 0);
        gpio_free(3 /*PA3*/);
    }

    if (gpio_request(14 /*PA14*/, "SR_STR_CPU") == -1)
    {
        printf("init_board_gpios_ios() - GPIO request failed for PA14 (14)\n");
    }
    else
    {
        gpio_direction_output(14 /*PA14*/, 0);
        gpio_free(14 /*PA14*/);
    }

    if (gpio_request(57 /*PD9*/, "SR_LD_IN") == -1)
    {
        printf("init_board_gpios_ios() - GPIO request failed for PD9 (57)\n");
    }
    else
    {
        gpio_direction_output(57 /*PD9*/, 0);
        gpio_free(57 /*PD9*/);
    }

    if (gpio_request(64 /*PE0*/, "AT_ZON_SET") == -1)
    {
        printf("init_board_gpios_ios() - GPIO request failed for PE0 (64)\n");
    }
    else
    {
        gpio_direction_output(64 /*PE0*/, 0);
        gpio_free(64 /*PE0*/);
    }

    if (gpio_request(111 /*PG15*/, "AT_ZON_RES") == -1)
    {
        printf("init_board_gpios_ios() - GPIO request failed for PG15 (111)\n");
    }
    else
    {
        gpio_direction_output(111 /*PG15*/, 0);
        gpio_free(111 /*PG15*/);
    }

    if (gpio_request(98 /*PG2*/, "EN_BR_SC") == -1)
    {
        printf("init_board_gpios_ios() - GPIO request failed for PG2 (98)\n");
    }
    else
    {
        gpio_direction_output(98 /*PG2*/, 0);
        gpio_free(98 /*PG2*/);
    }

    if (gpio_request(138 /*PI10*/, "SR_EXP_STR") == -1)
    {
        printf("init_board_gpios_ios() - GPIO request failed for PI10 (138)\n");
    }
    else
    {
        gpio_direction_output(138 /*PI10*/, 0);
        gpio_free(138 /*PI10*/);
    }

    if (gpio_request(88 /*PF8*/, "CAN1_STAT") == -1)
    {
        printf("init_board_gpios_ios() - GPIO request failed for PF8 (88)\n");
    }
    else
    {
        gpio_direction_output(88 /*PF8*/, 0);
        gpio_free(88 /*PF8*/);
    }

    if (gpio_request(87 /*PF7*/, "CAN1_ERR") == -1)
    {
        printf("init_board_gpios_ios() - GPIO request failed for PF7 (87)\n");
    }
    else
    {
        gpio_direction_output(87 /*PF7*/, 0);
        gpio_free(87 /*PF7*/);
    }

    if (gpio_request(106 /*PG10*/, "CAN2_STAT") == -1)
    {
        printf("init_board_gpios_ios() - GPIO request failed for PG10 (106)\n");
    }
    else
    {
        gpio_direction_output(106 /*PG10*/, 0);
        gpio_free(106 /*PG10*/);
    }

    if (gpio_request(103 /*PG7*/, "CAN2_ERR") == -1)
    {
        printf("init_board_gpios_ios() - GPIO request failed for PG7 (103)\n");
    }
    else
    {
        gpio_direction_output(103 /*PG7*/, 0);
        gpio_free(103 /*PG7*/);
    }

    if (gpio_request(15 /*PA15*/, "DCP_DIR") == -1)
    {
        printf("init_board_gpios_ios() - GPIO request failed for PA15 (15)\n");
    }
    else
    {
        gpio_direction_output(15 /*PA15*/, 0);
        gpio_free(15 /*PA15*/);
    }

    /* Inputs */

    if (gpio_request(66 /*PE2*/, "MON_SB_PRES") == -1)
    {
        printf("init_board_gpios_ios() - GPIO request failed for PE2 (66)\n");
    }
    else
    {
        gpio_direction_input(66 /*PE2*/);
        gpio_free(66 /*PE2*/);
    }

    if (gpio_request(6 /*PA6*/, "MON_SZ_PRES") == -1)
    {
        printf("init_board_gpios_ios() - GPIO request failed for PA6 (6)\n");
    }
    else
    {
        gpio_direction_input(6 /*PA6*/);
        gpio_free(6 /*PA6*/);
    }

    if (gpio_request(34 /*PC2*/, "SR_EXP_DO") == -1)
    {
        printf("init_board_gpios_ios() - GPIO request failed for PC2 (34)\n");
    }
    else
    {
        gpio_direction_input(34 /*PC2*/);
        gpio_free(34 /*PC2*/);
    }

    if (gpio_request(100 /*PG4*/, "PTC_MON") == -1)
    {
        printf("init_board_gpios_ios() - GPIO request failed for PG4 (100)\n");
    }
    else
    {
        gpio_direction_input(100 /*PG4*/);
        gpio_free(100 /*PG4*/);
    }

    if (gpio_request(35 /*PC3*/, "SR_DO_CPU") == -1)
    {
        printf("init_board_gpios_ios() - GPIO request failed for PC3 (35)\n");
    }
    else
    {
        gpio_direction_input(35 /*PC3*/);
        gpio_free(35 /*PC3*/);
    }

    if (gpio_request(96 /*PG0*/, "DZ_SENSOR") == -1)
    {
        printf("init_board_gpios_ios() - GPIO request failed for PG0 (96)\n");
    }
    else
    {
        gpio_direction_input(96 /*PG0*/);
        gpio_free(96 /*PG0*/);
    }

    if (gpio_request(97 /*PG1*/, "DOORS_BRIDGED") == -1)
    {
        printf("init_board_gpios_ios() - GPIO request failed for PG1 (97)\n");
    }
    else
    {
        gpio_direction_input(97 /*PG1*/);
        gpio_free(97 /*PG1*/);
    }

    if (gpio_request(406 /*PZ6*/, "CTP_RST") == -1)
    {
        printf("init_board_gpios_ios() - GPIO request failed for PZ6 (406)\n");
    }
    else
    {
        gpio_direction_output(406 /*PZ6*/, 0);
        gpio_free(406 /*PZ6*/);
    }

    if (gpio_request(401 /*PZ1*/, "CTP_INT") == -1)
    {
        printf("init_board_gpios_ios() - GPIO request failed for PZ1 (401)\n");
    }
    else
    {
        gpio_direction_input(401 /*PZ1*/);
        gpio_free(401 /*PZ1*/);
    }

    if (gpio_request(101 /*PG8*/, "AUDIO_SHDN") == -1)
    {
        printf("init_board_gpios_ios() - GPIO request failed for PG8 (101)\n");
    }
    else
    {
        gpio_direction_output(101 /*PG8*/, 0);
        gpio_free(101 /*PG8*/);
    }
}

int board_early_init_f(void)
{
	/* nothing to do, only used in SPL */
	return 0;
}

int checkboard(void)
{
	int ret;
	char *mode;
	u32 otp;
	struct udevice *dev;
	const char *fdt_compat;
	int fdt_compat_len;

	if (IS_ENABLED(CONFIG_STM32MP15x_STM32IMAGE))
		mode = "trusted - stm32image";
	else if (IS_ENABLED(CONFIG_TFABOOT))
		mode = "trusted";
	else
		mode = "basic";

	printf("Thor-E2 board: stm32mp1 in %s mode", mode);
	fdt_compat = fdt_getprop(gd->fdt_blob, 0, "compatible",
				 &fdt_compat_len);
	if (fdt_compat && fdt_compat_len)
		printf(" (%s)", fdt_compat);
	puts("\n");

	/* display the STMicroelectronics board identification */
	if (CONFIG_IS_ENABLED(CMD_STBOARD)) {
		ret = uclass_get_device_by_driver(UCLASS_MISC,
						  DM_GET_DRIVER(stm32mp_bsec),
						  &dev);
		if (!ret)
			ret = misc_read(dev, STM32_BSEC_SHADOW(BSEC_OTP_BOARD),
					&otp, sizeof(otp));
		if (ret > 0 && otp)
			printf("Board: MB%04x Var%d.%d Rev.%c-%02d\n",
			       otp >> 16,
			       (otp >> 12) & 0xF,
			       (otp >> 4) & 0xF,
			       ((otp >> 8) & 0xF) - 1 + 'A',
			       otp & 0xF);
	}

	return 0;
}

int g_dnl_board_usb_cable_connected(void)
{
	struct udevice *dwc2_udc_otg;
	int ret;

	if (!IS_ENABLED(CONFIG_USB_GADGET_DWC2_OTG))
		return -ENODEV;

	/* if typec stusb160x is present, means DK1 or DK2 board */
	ret = stusb160x_cable_connected();
	if (ret >= 0)
		return ret;

	ret = uclass_get_device_by_driver(UCLASS_USB_GADGET_GENERIC,
					  DM_GET_DRIVER(dwc2_udc_otg),
					  &dwc2_udc_otg);
	if (!ret)
		debug("dwc2_udc_otg init failed\n");

	return dwc2_udc_B_session_valid(dwc2_udc_otg);
}

#ifdef CONFIG_USB_GADGET_DOWNLOAD
#define STM32MP1_G_DNL_DFU_PRODUCT_NUM 0xdf11
#define STM32MP1_G_DNL_FASTBOOT_PRODUCT_NUM 0x0afb

int g_dnl_bind_fixup(struct usb_device_descriptor *dev, const char *name)
{
	if (IS_ENABLED(CONFIG_DFU_OVER_USB) &&
	    !strcmp(name, "usb_dnl_dfu"))
		put_unaligned(STM32MP1_G_DNL_DFU_PRODUCT_NUM, &dev->idProduct);
	else if (IS_ENABLED(CONFIG_FASTBOOT) &&
		 !strcmp(name, "usb_dnl_fastboot"))
		put_unaligned(STM32MP1_G_DNL_FASTBOOT_PRODUCT_NUM,
			      &dev->idProduct);
	else
		put_unaligned(CONFIG_USB_GADGET_PRODUCT_NUM, &dev->idProduct);

	return 0;
}
#endif /* CONFIG_USB_GADGET_DOWNLOAD */

static void sysconf_init(void)
{
	u8 *syscfg;
	struct udevice *pwr_dev;
	struct udevice *pwr_reg;
	struct udevice *dev;
	u32 otp = 0;
	int ret;
	u32 bootr, val;

	syscfg = (u8 *)syscon_get_first_range(STM32MP_SYSCON_SYSCFG);

	/* interconnect update : select master using the port 1 */
	/* LTDC = AXI_M9 */
	/* GPU  = AXI_M8 */
	/* today information is hardcoded in U-Boot */
	writel(BIT(9), syscfg + SYSCFG_ICNR);

	/* disable Pull-Down for boot pin connected to VDD */
	bootr = readl(syscfg + SYSCFG_BOOTR);
	bootr &= ~(SYSCFG_BOOTR_BOOT_MASK << SYSCFG_BOOTR_BOOTPD_SHIFT);
	bootr |= (bootr & SYSCFG_BOOTR_BOOT_MASK) << SYSCFG_BOOTR_BOOTPD_SHIFT;
	writel(bootr, syscfg + SYSCFG_BOOTR);

	/* High Speed Low Voltage Pad mode Enable for SPI, SDMMC, ETH, QSPI
	 * and TRACE. Needed above ~50MHz and conditioned by AFMUX selection.
	 * The customer will have to disable this for low frequencies
	 * or if AFMUX is selected but the function not used, typically for
	 * TRACE. Otherwise, impact on power consumption.
	 *
	 * WARNING:
	 *   enabling High Speed mode while VDD>2.7V
	 *   with the OTP product_below_2v5 (OTP 18, BIT 13)
	 *   erroneously set to 1 can damage the IC!
	 *   => U-Boot set the register only if VDD < 2.7V (in DT)
	 *      but this value need to be consistent with board design
	 */
	ret = uclass_get_device_by_driver(UCLASS_PMIC,
					  DM_GET_DRIVER(stm32mp_pwr_pmic),
					  &pwr_dev);
	if (!ret && IS_ENABLED(CONFIG_DM_REGULATOR)) {
		ret = uclass_get_device_by_driver(UCLASS_MISC,
						  DM_GET_DRIVER(stm32mp_bsec),
						  &dev);
		if (ret) {
			pr_err("Can't find stm32mp_bsec driver\n");
			return;
		}

		ret = misc_read(dev, STM32_BSEC_SHADOW(18), &otp, 4);
		if (ret > 0)
			otp = otp & BIT(13);

		/* get VDD = vdd-supply */
		ret = device_get_supply_regulator(pwr_dev, "vdd-supply",
						  &pwr_reg);

		/* check if VDD is Low Voltage */
		if (!ret) {
			if (regulator_get_value(pwr_reg) < 2700000) {
				writel(SYSCFG_IOCTRLSETR_HSLVEN_TRACE |
				       SYSCFG_IOCTRLSETR_HSLVEN_QUADSPI |
				       SYSCFG_IOCTRLSETR_HSLVEN_ETH |
				       SYSCFG_IOCTRLSETR_HSLVEN_SDMMC |
				       SYSCFG_IOCTRLSETR_HSLVEN_SPI,
				       syscfg + SYSCFG_IOCTRLSETR);

				if (!otp)
					pr_err("product_below_2v5=0: HSLVEN protected by HW\n");
			} else {
				if (otp)
					pr_err("product_below_2v5=1: HSLVEN update is destructive, no update as VDD>2.7V\n");
			}
		} else {
			debug("VDD unknown");
		}
	}

	/* activate automatic I/O compensation
	 * warning: need to ensure CSI enabled and ready in clock driver
	 */
	writel(SYSCFG_CMPENSETR_MPU_EN, syscfg + SYSCFG_CMPENSETR);

	/* poll until ready (1s timeout) */
	ret = readl_poll_timeout(syscfg + SYSCFG_CMPCR, val,
				 val & SYSCFG_CMPCR_READY,
				 1000000);
	if (ret) {
		pr_err("SYSCFG: I/O compensation failed, timeout.\n");
	}

	clrbits_le32(syscfg + SYSCFG_CMPCR, SYSCFG_CMPCR_SW_CTRL);
}

/* board dependent setup after realloc */
int board_init(void)
{
	/* address of boot parameters */
	gd->bd->bi_boot_params = STM32_DDR_BASE + 0x100;

	if (IS_ENABLED(CONFIG_DM_REGULATOR))
		regulators_enable_boot_on(_DEBUG);

	if (!IS_ENABLED(CONFIG_TFABOOT))
		sysconf_init();

        /* THOR shift register IO initialization. */

	init_board_gpios_ios();

        /* THOR display initialization. */

	init_gc9503v_display_cntrl();

	return 0;
}

int board_late_init(void)
{
	const void *fdt_compat;
	int fdt_compat_len;
	int ret;
	u32 otp;
	struct udevice *dev;
	char buf[10];
	char dtb_name[256];
	int buf_len;

	if (IS_ENABLED(CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG)) {
		fdt_compat = fdt_getprop(gd->fdt_blob, 0, "compatible",
					 &fdt_compat_len);
		if (fdt_compat && fdt_compat_len) {
			if (strncmp(fdt_compat, "st,", 3) != 0) {
				env_set("board_name", fdt_compat);
			} else {
				env_set("board_name", fdt_compat + 3);

				buf_len = sizeof(dtb_name);
				strncpy(dtb_name, fdt_compat + 3, buf_len);
				buf_len -= strlen(fdt_compat + 3);
				strncat(dtb_name, ".dtb", buf_len);
				env_set("fdtfile", dtb_name);
			}
		}
		ret = uclass_get_device_by_driver(UCLASS_MISC,
						  DM_GET_DRIVER(stm32mp_bsec),
						  &dev);

		if (!ret)
			ret = misc_read(dev, STM32_BSEC_SHADOW(BSEC_OTP_BOARD),
					&otp, sizeof(otp));
		if (ret > 0 && otp) {
			snprintf(buf, sizeof(buf), "0x%04x", otp >> 16);
			env_set("board_id", buf);

			snprintf(buf, sizeof(buf), "0x%04x",
				 ((otp >> 8) & 0xF) - 1 + 0xA);
			env_set("board_rev", buf);
		}
	}

	return 0;
}

void board_quiesce_devices(void)
{
}

/* CLOCK feed to PHY*/
#define ETH_CK_F_25M	25000000
#define ETH_CK_F_50M	50000000
#define ETH_CK_F_125M	125000000

struct stm32_syscfg_pmcsetr {
	u32 syscfg_clr_off;
	u32 eth1_clk_sel;
	u32 eth1_ref_clk_sel;
	u32 eth1_sel_mii;
	u32 eth1_sel_rgmii;
	u32 eth1_sel_rmii;
	u32 eth2_clk_sel;
	u32 eth2_ref_clk_sel;
	u32 eth2_sel_rgmii;
	u32 eth2_sel_rmii;
};

const struct stm32_syscfg_pmcsetr stm32mp15_syscfg_pmcsetr = {
	.syscfg_clr_off		= 0x44,
	.eth1_clk_sel		= BIT(16),
	.eth1_ref_clk_sel	= BIT(17),
	.eth1_sel_mii		= BIT(20),
	.eth1_sel_rgmii		= BIT(21),
	.eth1_sel_rmii		= BIT(23),
	.eth2_clk_sel		= 0,
	.eth2_ref_clk_sel	= 0,
	.eth2_sel_rgmii		= 0,
	.eth2_sel_rmii		= 0
};

#define SYSCFG_PMCSETR_ETH_MASK		GENMASK(23, 16)
#define SYSCFG_PMCR_ETH_SEL_GMII	0

/* eth init function : weak called in eqos driver */
int board_interface_eth_init(struct udevice *dev,
			     phy_interface_t interface_type, ulong rate)
{
	struct regmap *regmap;
	uint regmap_mask;
	int ret;
	u32 value;
	bool ext_phyclk, eth_clk_sel_reg, eth_ref_clk_sel_reg;
	const struct stm32_syscfg_pmcsetr *pmcsetr;

	/* Ethernet PHY have no crystal */
	ext_phyclk = dev_read_bool(dev, "st,ext-phyclk");

	/* Gigabit Ethernet 125MHz clock selection. */
	eth_clk_sel_reg = dev_read_bool(dev, "st,eth-clk-sel");

	/* Ethernet 50Mhz RMII clock selection */
	eth_ref_clk_sel_reg = dev_read_bool(dev, "st,eth-ref-clk-sel");

	pmcsetr = &stm32mp15_syscfg_pmcsetr;

	regmap = syscon_regmap_lookup_by_phandle(dev, "st,syscon");
	if (!IS_ERR(regmap)) {
		u32 fmp[3];

		ret = dev_read_u32_array(dev, "st,syscon", fmp, 3);
		if (ret)
			/*  If no mask in DT, it is MP15 (backward compatibility) */
			regmap_mask = SYSCFG_PMCSETR_ETH_MASK;
		else
			regmap_mask = fmp[2];
	} else {
		return -ENODEV;
	}

	switch (interface_type) {
	case PHY_INTERFACE_MODE_MII:
		value = pmcsetr->eth1_sel_mii;
		log_debug("PHY_INTERFACE_MODE_MII\n");
		break;
	case PHY_INTERFACE_MODE_GMII:
		value = SYSCFG_PMCR_ETH_SEL_GMII;
		log_debug("PHY_INTERFACE_MODE_GMII\n");
		break;
	case PHY_INTERFACE_MODE_RMII:
		value = pmcsetr->eth1_sel_rmii;
		if (rate == ETH_CK_F_50M && (eth_clk_sel_reg || ext_phyclk))
			value |= pmcsetr->eth1_ref_clk_sel;
		log_debug("PHY_INTERFACE_MODE_RMII\n");
		break;
	case PHY_INTERFACE_MODE_RGMII:
	case PHY_INTERFACE_MODE_RGMII_ID:
	case PHY_INTERFACE_MODE_RGMII_RXID:
	case PHY_INTERFACE_MODE_RGMII_TXID:
		value = pmcsetr->eth1_sel_rgmii;
		if (rate == ETH_CK_F_125M && (eth_clk_sel_reg || ext_phyclk))
			value |= pmcsetr->eth1_clk_sel;
		log_debug("PHY_INTERFACE_MODE_RGMII\n");
		break;
	default:
		log_debug("Do not manage %d interface\n", interface_type);
		/* Do not manage others interfaces */
		return -EINVAL;
	}

	/* Need to update PMCCLRR (clear register) */
	regmap_write(regmap, pmcsetr->syscfg_clr_off, regmap_mask);

	ret = regmap_update_bits(regmap, SYSCFG_PMCSETR, regmap_mask, value);

	return ret;
}

enum env_location env_get_location(enum env_operation op, int prio)
{
	u32 bootmode = get_bootmode();

	if (prio)
		return ENVL_UNKNOWN;

	switch (bootmode & TAMP_BOOT_DEVICE_MASK) {
	case BOOT_FLASH_SD:
	case BOOT_FLASH_EMMC:
		if (CONFIG_IS_ENABLED(ENV_IS_IN_MMC))
			return ENVL_MMC;
		else if (CONFIG_IS_ENABLED(ENV_IS_IN_EXT4))
			return ENVL_EXT4;
		else
			return ENVL_NOWHERE;

	case BOOT_FLASH_NAND:
	case BOOT_FLASH_SPINAND:
		if (CONFIG_IS_ENABLED(ENV_IS_IN_UBI))
			return ENVL_UBI;
		else
			return ENVL_NOWHERE;

	case BOOT_FLASH_NOR:
		if (CONFIG_IS_ENABLED(ENV_IS_IN_SPI_FLASH))
			return ENVL_SPI_FLASH;
		else
			return ENVL_NOWHERE;

	default:
		return ENVL_NOWHERE;
	}
}

const char *env_ext4_get_intf(void)
{
	u32 bootmode = get_bootmode();

	switch (bootmode & TAMP_BOOT_DEVICE_MASK) {
	case BOOT_FLASH_SD:
	case BOOT_FLASH_EMMC:
		return "mmc";
	default:
		return "";
	}
}

const char *env_ext4_get_dev_part(void)
{
	static char *const dev_part[] = {"0:auto", "1:auto", "2:auto"};
	u32 bootmode = get_bootmode();

	return dev_part[(bootmode & TAMP_BOOT_INSTANCE_MASK) - 1];
}

int mmc_get_env_dev(void)
{
	u32 bootmode;

	if (CONFIG_SYS_MMC_ENV_DEV >= 0)
		return CONFIG_SYS_MMC_ENV_DEV;

	bootmode = get_bootmode();

	/* use boot instance to select the correct mmc device identifier */
	return (bootmode & TAMP_BOOT_INSTANCE_MASK) - 1;
}

#if defined(CONFIG_OF_BOARD_SETUP)
int ft_board_setup(void *blob, struct bd_info *bd)
{
	static const struct node_info nodes[] = {
		{ "st,stm32f469-qspi",		MTD_DEV_TYPE_NOR,  },
		{ "st,stm32f469-qspi",		MTD_DEV_TYPE_SPINAND},
		{ "st,stm32mp15-fmc2",		MTD_DEV_TYPE_NAND, },
		{ "st,stm32mp1-fmc2-nfc",	MTD_DEV_TYPE_NAND, },
	};
	char *boot_device;

	/* Check the boot-source and don't update MTD for serial or usb boot */
	boot_device = env_get("boot_device");
	if (!boot_device ||
	    (strcmp(boot_device, "serial") && strcmp(boot_device, "usb")))
		if (IS_ENABLED(CONFIG_FDT_FIXUP_PARTITIONS))
			fdt_fixup_mtdparts(blob, nodes, ARRAY_SIZE(nodes));

	if (CONFIG_IS_ENABLED(FDT_SIMPLEFB))
		fdt_simplefb_add_node_and_mem_rsv(blob);

	return 0;
}
#endif

static void board_copro_image_process(ulong fw_image, size_t fw_size)
{
	int ret, id = 0; /* Copro id fixed to 0 as only one coproc on mp1 */

	if (!rproc_is_initialized())
		if (rproc_init()) {
			printf("Remote Processor %d initialization failed\n",
			       id);
			return;
		}

	ret = rproc_load(id, fw_image, fw_size);
	printf("Load Remote Processor %d with data@addr=0x%08lx %u bytes:%s\n",
	       id, fw_image, fw_size, ret ? " Failed!" : " Success!");

	if (!ret)
		rproc_start(id);
}

U_BOOT_FIT_LOADABLE_HANDLER(IH_TYPE_COPRO, board_copro_image_process);
