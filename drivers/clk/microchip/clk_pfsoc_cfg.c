// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2020 Microchip Technology Inc.
 * Padmarao Begari <padmarao.begari@microchip.com>
 */
#include <common.h>
#include <clk.h>
#include <clk-uclass.h>
#include <dm/device.h>
#include <dm/devres.h>
#include <dm/uclass.h>
#include <asm/io.h>
#include <linux/err.h>
#include <dt-bindings/clock/microchip,pfsoc-clock.h>

#include "clk_pfsoc.h"

#define PFSOC_CFG_CLOCK "pfsoc_cfg_clock"

#define REG_CLOCK_CONFIG_CR 0x08

static const struct clk_div_table pfsoc_div_cpu_axi_table[] = {
	{ 0, 1 }, { 1, 2 }, { 2, 4 }, { 3, 8 },
	{ 0, 0 }
};

static const struct clk_div_table pfsoc_div_ahb_table[] = {
	{ 1, 2 }, { 2, 4}, { 3, 8 },
	{ 0, 0 }
};

struct pfsoc_cfg_clock {
	unsigned int id;
	const char *name;
	u8 shift;
	u8 width;
	const struct clk_div_table *table;
	unsigned long flags;
};

struct pfsoc_cfg_hw_clock {
	struct pfsoc_cfg_clock cfg;
	void __iomem *sys_base;
	u32 prate;
	struct clk hw;
};

#define to_pfsoc_cfg_clk(_hw) container_of(_hw, struct pfsoc_cfg_hw_clock, hw)

static unsigned long pfsoc_cfg_clk_recalc_rate(struct clk *hw)
{
	struct pfsoc_cfg_hw_clock *cfg_hw = to_pfsoc_cfg_clk(hw);
	struct pfsoc_cfg_clock *cfg = &cfg_hw->cfg;
	void __iomem *base_addr = cfg_hw->sys_base;
	unsigned long rate;
	u32 val;

	val = readl(base_addr + REG_CLOCK_CONFIG_CR) >> cfg->shift;
	val &= clk_div_mask(cfg->width);
	rate = cfg_hw->prate / (1u << val);
	hw->rate = rate;

	return rate;
}

static ulong pfsoc_cfg_clk_set_rate(struct clk *hw, ulong rate)
{
	struct pfsoc_cfg_hw_clock *cfg_hw = to_pfsoc_cfg_clk(hw);
	struct pfsoc_cfg_clock *cfg = &cfg_hw->cfg;
	void __iomem *base_addr = cfg_hw->sys_base;
	u32  val;
	int divider_setting;

	divider_setting = divider_get_val(rate, cfg_hw->prate, cfg->table, cfg->width, cfg->flags);

	if (divider_setting < 0)
		return divider_setting;

	val = readl(base_addr + REG_CLOCK_CONFIG_CR);
	val &= ~(clk_div_mask(cfg->width) << cfg_hw->cfg.shift);
	val |= divider_setting << cfg->shift;
	writel(val, base_addr + REG_CLOCK_CONFIG_CR);

	return 0;
}

#define CLK_CFG(_id, _name, _shift, _width, _table, _flags) {	\
		.cfg.id = _id,					\
		.cfg.name = _name,				\
		.cfg.shift = _shift,				\
		.cfg.width = _width,				\
		.cfg.table = _table,				\
		.cfg.flags = _flags,				\
	}

static struct pfsoc_cfg_hw_clock pfsoc_cfg_clks[] = {
	CLK_CFG(CLK_CPU, "clk_cpu", 0, 2, pfsoc_div_cpu_axi_table, 0),
	CLK_CFG(CLK_AXI, "clk_axi", 2, 2, pfsoc_div_cpu_axi_table, 0),
	CLK_CFG(CLK_AHB, "clk_ahb", 4, 2, pfsoc_div_ahb_table, 0),
};

int pfsoc_clk_register_cfgs(void __iomem *base, u32 clk_rate,
				const char *parent_name)
{
	int ret;
	int i, id, num_clks;
	const char *name;
	struct clk *hw;

	num_clks = ARRAY_SIZE(pfsoc_cfg_clks);

	for (i = 0; i < num_clks; i++) {
		hw = &pfsoc_cfg_clks[i].hw;
		pfsoc_cfg_clks[i].sys_base = base;
		pfsoc_cfg_clks[i].prate = clk_rate;
		name = pfsoc_cfg_clks[i].cfg.name;
		ret = clk_register(hw, PFSOC_CFG_CLOCK, name, parent_name);
		if (ret)
			ERR_PTR(ret);
		id = pfsoc_cfg_clks[i].cfg.id;
		clk_dm(id, hw);
	}
	return 0;
}

const struct clk_ops pfsoc_cfg_clk_ops = {
	.set_rate = pfsoc_cfg_clk_set_rate,
	.get_rate = pfsoc_cfg_clk_recalc_rate,
};

U_BOOT_DRIVER(pfsoc_cfg_clock) = {
	.name	= PFSOC_CFG_CLOCK,
	.id	= UCLASS_CLK,
	.ops	= &pfsoc_cfg_clk_ops,
};
