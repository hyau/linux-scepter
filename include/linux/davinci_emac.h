/*
 * TI DaVinci EMAC platform support
 *
 * Author: Kevin Hilman, Deep Root Systems, LLC
 *
 * 2007 (c) Deep Root Systems, LLC. This file is licensed under
 * the terms of the GNU General Public License version 2. This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */
#ifndef _LINUX_DAVINCI_EMAC_H
#define _LINUX_DAVINCI_EMAC_H

#include <linux/if_ether.h>
#include <linux/phy.h>
#include <linux/memory.h>

struct emac_mdio_data {
	u32 regs;
	u32 size;
	u32 max_freq;
};

struct emac_platform_data {
	char mac_addr[ETH_ALEN];
	u32 ctrl_reg_offset;
	u32 ctrl_mod_reg_offset;
	u32 ctrl_ram_offset;
	u32 hw_ram_addr;
	u32 mdio_reg_offset;
	u32 ctrl_ram_size;
	u32 phy_mask;
	u32 mdio_max_freq;
	u8 rmii_en;
	u8 version;
	void (*interrupt_enable) (void);
	void (*interrupt_disable) (void);
	struct emac_mdio_data *mdio_data;
};

enum {
	EMAC_VERSION_1,	/* DM644x */
	EMAC_VERSION_2,	/* DM646x */
};

void davinci_get_mac_addr(struct memory_accessor *mem_acc, void *context);
#endif
