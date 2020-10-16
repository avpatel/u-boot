// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2020 Microchip Technology Inc.
 * Padmarao Begari <padmarao.begari@microchip.com>
 */
#include <common.h>
#include <clk.h>
#include <clk-uclass.h>
#include <dm.h>
#include <log.h>
#include <dm/device.h>
#include <dm/devres.h>
#include <dm/uclass.h>
#include <malloc.h>
#include <linux/err.h>

#include "clk_pfsoc.h"

/* All methods are delegated to CCF clocks */

static ulong pfsoc_clk_get_rate(struct clk *clk)
{
	struct clk *c;
	int err = clk_get_by_id(clk->id, &c);

	if (err)
		return err;
	return clk_get_rate(c);
}

static ulong pfsoc_clk_set_rate(struct clk *clk, unsigned long rate)
{
	struct clk *c;
	int err = clk_get_by_id(clk->id, &c);

	if (err)
		return err;
	return clk_set_rate(c, rate);
}

static int pfsoc_clk_set_parent(struct clk *clk, struct clk *parent)
{
	struct clk *c, *p;
	int err = clk_get_by_id(clk->id, &c);

	if (err)
		return err;

	err = clk_get_by_id(parent->id, &p);
	if (err)
		return err;

	return clk_set_parent(c, p);
}

static int pfsoc_clk_endisable(struct clk *clk, bool enable)
{
	struct clk *c;
	int err = clk_get_by_id(clk->id, &c);

	if (err)
		return err;
	return enable ? clk_enable(c) : clk_disable(c);
}

static int pfsoc_clk_enable(struct clk *clk)
{
	return pfsoc_clk_endisable(clk, true);
}

static int pfsoc_clk_disable(struct clk *clk)
{
	return pfsoc_clk_endisable(clk, false);
}

static int pfsoc_clk_probe(struct udevice *dev)
{
	int ret;
	void __iomem *base;
	u32 clk_rate;
	struct clk *clk;
	const char *parent_clk_name;

	base = dev_read_addr_ptr(dev);
	if (!base)
		return -ENODEV;
	clk = kzalloc(sizeof(*clk), GFP_KERNEL);
	if (!clk)
		return -ENOMEM;

	ret = clk_get_by_index(dev, 0, clk);
	if (ret)
		return ret;
	dev_read_u32(clk->dev, "clock-frequency", &clk_rate);
	parent_clk_name = clk->dev->name;
	ret = pfsoc_clk_register_cfgs(base, clk_rate, parent_clk_name);
	ret = pfsoc_clk_register_periphs(base, clk_rate, "clk_ahb");
	return ret;
}

static const struct clk_ops pfsoc_clk_ops = {
	.set_rate = pfsoc_clk_set_rate,
	.get_rate = pfsoc_clk_get_rate,
	.set_parent = pfsoc_clk_set_parent,
	.enable = pfsoc_clk_enable,
	.disable = pfsoc_clk_disable,
};

static const struct udevice_id pfsoc_of_match[] = {
	{ .compatible = "microchip,pfsoc-clkcfg" },
	{ }
};

U_BOOT_DRIVER(pfsoc_clk) = {
	.name = "pfsoc_clk",
	.id = UCLASS_CLK,
	.of_match = pfsoc_of_match,
	.ops = &pfsoc_clk_ops,
	.probe = pfsoc_clk_probe,
};
