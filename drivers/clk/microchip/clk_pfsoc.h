// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2020 Microchip Technology Inc.
 * Padmarao Begari <padmarao.begari@microchip.com>
 */
#ifndef __MICROCHIP_PFSOC_CLK_H
#define __MICROCHIP_PFSOC_CLK_H

#include <linux/clk-provider.h>

int pfsoc_clk_register_cfgs(void __iomem *base, u32 clk_rate,
					const char *parent_name);
int pfsoc_clk_register_periphs(void __iomem *base, u32 clk_rate,
					const char *parent_name);
int divider_get_val(unsigned long rate, unsigned long parent_rate,
					const struct clk_div_table *table,
					u8 width, unsigned long flags);

#endif	/* __MICROCHIP_PFSOC_CLK_H */
