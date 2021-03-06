/*
 * arch/arm/mach-spear13xx/include/mach/spear.h
 *
 * spear13xx Machine family specific definition
 *
 * Copyright (C) 2010 ST Microelectronics
 * Shiraz Hashim <shiraz.hashim@st.com>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#ifndef __MACH_SPEAR13XX_H
#define __MACH_SPEAR13XX_H

#include <asm/memory.h>
#include <mach/spear1310.h>

#define SPEAR13XX_L2CC_BASE		UL(0xED000000)

/* ARM9SMP peripheral base address */
#define SPEAR13XX_A9SM_PERIP_BASE	UL(0xEC800000)
#define VA_SPEAR13XX_A9SM_PERIP_BASE	IO_ADDRESS(SPEAR13XX_A9SM_PERIP_BASE)
/* A9SM peripheral offsets */
#define SPEAR13XX_SCU_BASE		(SPEAR13XX_A9SM_PERIP_BASE + 0x00)
#define SPEAR13XX_GIC_CPU_BASE		(SPEAR13XX_A9SM_PERIP_BASE + 0x100)
#define SPEAR13XX_GLOBAL_TMR_BASE	(SPEAR13XX_A9SM_PERIP_BASE + 0x200)
#define SPEAR13XX_LOCAL_TMR_BASE	(SPEAR13XX_A9SM_PERIP_BASE + 0x600)
#define SPEAR13XX_WDT_BASE		(SPEAR13XX_A9SM_PERIP_BASE + 0x620)
#define SPEAR13XX_GIC_DIST_BASE		(SPEAR13XX_A9SM_PERIP_BASE + 0x1000)

#define SPEAR13XX_UART_BASE		UL(0xE0000000)
#define VA_SPEAR13XX_UART_BASE		IO_ADDRESS(SPEAR13XX_UART_BASE)

#define SPEAR13XX_ADC_BASE		UL(0xE0080000)
#define SPEAR13XX_SSP_BASE		UL(0xE0100000)
#define SPEAR13XX_I2S0_BASE		UL(0xE0180000)
#define SPEAR13XX_I2S1_BASE		UL(0xE0200000)
#define SPEAR13XX_I2C_BASE		UL(0xE0280000)
#define SPEAR13XX_KBD_BASE		UL(0xE0300000)
#define SPEAR13XX_GPT0_BASE		UL(0xE0380000)
#define SPEAR13XX_GPT1_BASE		UL(0xE0400000)
#define SPEAR13XX_GPT2_BASE		UL(0xE0480000)
#define SPEAR13XX_GPT3_BASE		UL(0xE0500000)
#define SPEAR13XX_RTC_BASE		UL(0xE0580000)
#define SPEAR13XX_GPIO0_BASE		UL(0xE0600000)
#define SPEAR13XX_GPIO1_BASE		UL(0xE0680000)
#define SPEAR13XX_MISC_BASE		UL(0xE0700000)
#define VA_SPEAR13XX_MISC_BASE		IO_ADDRESS(SPEAR13XX_MISC_BASE)

#define SPEAR13XX_SYSRAM0_BASE		UL(0xB3800000)
#define SPEAR13XX_SYSRAM0_SIZE		0x00800000

/*
 * The system location which is polled by secondary cpus to find the
 * jump address
 */
#define SPEAR13XX_SYS_LOCATION		(SPEAR13XX_SYSRAM0_BASE + 0x600)

#define SPEAR13XX_SYSRAM1_BASE		UL(0xE0800000)
#define SPEAR13XX_SYSRAM1_SIZE		0x00001000
#define SPEAR13XX_CLCD_BASE		UL(0xE1000000)
#define SPEAR13XX_DB9000_LCD_BASE	UL(0xE1000000)
#define SPEAR13XX_C3_BASE		UL(0xE1800000)
#define SPEAR13XX_GETH0_BASE		UL(0xE2000000)
#define SPEAR13XX_UPD_BASE		UL(0xE2800000)
#define SPEAR13XX_UDC_BASE		UL(0xE3800000)
#define SPEAR13XX_UHC0_OHCI_BASE	UL(0xE4000000)
#define SPEAR13XX_UHC0_EHCI_BASE	UL(0xE4800000)
#define SPEAR13XX_UHC1_OHCI_BASE	UL(0xE5000000)
#define SPEAR13XX_UHC1_EHCI_BASE	UL(0xE5800000)
#define SPEAR13XX_SMI_MEM_BASE		UL(0xE6000000)
#define SPEAR13XX_SMI_CTRL_BASE		UL(0xEA000000)
#define SPEAR13XX_DMAC0_BASE		UL(0xEA800000)
#define SPEAR13XX_DMAC1_BASE		UL(0xEB000000)
#define SPEAR13XX_MII_PHY_BASE		UL(0xEB800000)
#define SPEAR13XX_MPMC_BASE		UL(0xEC000000)
#define SPEAR13XX_PCIE0_BASE		UL(0x80000000)
#define SPEAR13XX_PCIE1_BASE		UL(0x90000000)
#define SPEAR13XX_PCIE2_BASE		UL(0xC0000000)
#define SPEAR13XX_PCIE0_APP_BASE	UL(0xB1000000)
#define SPEAR13XX_PCIE1_APP_BASE	UL(0xB1800000)
#define SPEAR13XX_PCIE2_APP_BASE	UL(0xB4000000)
#define SPEAR13XX_FSMC_MEM_BASE		UL(0xA0000000)
#define SPEAR13XX_FSMC_BASE		UL(0xB0000000)
#define SPEAR13XX_JPEG_BASE		UL(0xB2000000)
#define SPEAR13XX_MCIF_CF_BASE		UL(0xB2800000)
#define SPEAR13XX_MCIF_SDHCI_BASE	UL(0xB3000000)

/* Debug uart for linux, will be used for debug and uncompress messages */
#define SPEAR_DBG_UART_BASE		SPEAR13XX_UART_BASE
#define VA_SPEAR_DBG_UART_BASE		VA_SPEAR13XX_UART_BASE

#endif /* __MACH_SPEAR13XX_H */
