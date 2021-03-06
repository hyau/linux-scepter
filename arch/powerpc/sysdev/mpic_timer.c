/*
 * Copyright (c) 2008-2010 Freescale Semiconductor, Inc. All rights reserved.
 * Dave Liu <daveliu@freescale.com>
 * copy from the 83xx GTM driver and modify for MPIC global timer,
 * implement the global timer 0 function.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/interrupt.h>
#include <linux/sysfs.h>
#include <linux/of_platform.h>

#include <linux/io.h>
#include <linux/irq.h>

#include <sysdev/fsl_soc.h>

#define MPIC_TIMER_TCR_OFFSET		0x200
#define MPIC_TIMER_TCR_CLKDIV_64	0x00000300
#define MPIC_TIMER_STOP			0x80000000

struct mpic_tm_regs {
	u32	gtccr;
	u32	res0[3];
	u32	gtbcr;
	u32	res1[3];
	u32	gtvpr;
	u32	res2[3];
	u32	gtdr;
	u32	res3[3];
};

struct mpic_tm_priv {
	struct mpic_tm_regs __iomem *regs;
	int irq;
	int ticks_per_sec;
	spinlock_t lock;
};

struct mpic_type {
	int has_tcr;
};

static irqreturn_t mpic_tm_isr(int irq, void *dev_id)
{
	struct mpic_tm_priv *priv = dev_id;
	unsigned long flags;
	unsigned long temp;

	spin_lock_irqsave(&priv->lock, flags);
	temp = in_be32(&priv->regs->gtbcr);
	temp |= MPIC_TIMER_STOP; /* counting inhibited */
	out_be32(&priv->regs->gtbcr, temp);
	spin_unlock_irqrestore(&priv->lock, flags);

	return IRQ_HANDLED;
}

static ssize_t mpic_tm_timeout_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	struct mpic_tm_priv *priv = dev_get_drvdata(dev);
	unsigned long interval = simple_strtoul(buf, NULL, 0);
	unsigned long temp;

	if (interval > 0x7fffffff) {
		dev_dbg(dev, "mpic_tm: interval %lu (in s) too long\n", interval);
		return -EINVAL;
	}

	temp = interval;
	interval *= priv->ticks_per_sec;

	if (interval > 0x7fffffff || (interval / priv->ticks_per_sec) != temp) {
		dev_dbg(dev, "mpic_tm: interval %lu (in ticks) too long\n",
			interval);
		return -EINVAL;
	}

	spin_lock_irq(&priv->lock);

	/* stop timer 0 */
	temp = in_be32(&priv->regs->gtbcr);
	temp |= MPIC_TIMER_STOP; /* counting inhibited */
	out_be32(&priv->regs->gtbcr, temp);

	if (interval != 0) {
		/* start timer */
		out_be32(&priv->regs->gtbcr, interval | MPIC_TIMER_STOP);
		out_be32(&priv->regs->gtbcr, interval);
	}

	spin_unlock_irq(&priv->lock);
	return count;
}

static ssize_t mpic_tm_timeout_show(struct device *dev,
				struct device_attribute *attr,
				char *buf)
{
	struct mpic_tm_priv *priv = dev_get_drvdata(dev);
	int timeout = 0;

	spin_lock_irq(&priv->lock);

	if (!(in_be32(&priv->regs->gtbcr) & MPIC_TIMER_STOP)) {
		timeout = in_be32(&priv->regs->gtccr);
		timeout += priv->ticks_per_sec - 1;
		timeout /= priv->ticks_per_sec;
	}

	spin_unlock_irq(&priv->lock);
	return sprintf(buf, "%u\n", timeout);
}

static DEVICE_ATTR(timeout, 0660, mpic_tm_timeout_show, mpic_tm_timeout_store);

static int __devinit mpic_tm_probe(struct of_device *dev,
				const struct of_device_id *match)
{
	struct device_node *np = dev->node;
	struct resource res;
	struct mpic_tm_priv *priv;
	struct mpic_type *type = match->data;
	int has_tcr = type->has_tcr;
	u32 busfreq = fsl_get_sys_freq();
	int ret = 0;

	if (busfreq == 0) {
		dev_err(&dev->dev, "mpic_tm: No bus frequency in device tree.\n");
		return -ENODEV;
	}

	priv = kmalloc(sizeof(struct mpic_tm_priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	spin_lock_init(&priv->lock);
	dev_set_drvdata(&dev->dev, priv);

	ret = of_address_to_resource(np, 0, &res);
	if (ret)
		goto out;

	priv->irq = irq_of_parse_and_map(np, 0);
	if (priv->irq == NO_IRQ) {
		dev_err(&dev->dev, "MPIC global timer0 exists in device tree "
				"without an IRQ.\n");
		ret = -ENODEV;
		goto out;
	}

	ret = request_irq(priv->irq, mpic_tm_isr, 0, "mpic timer 0", priv);
	if (ret)
		goto out;

	priv->regs = ioremap(res.start, res.end - res.start + 1);
	if (!priv->regs) {
		ret = -ENOMEM;
		goto out;
	}

	/*
	 * MPIC implementation from Freescale has the TCR register,
	 * the MPIC_TIMER_TCR_OFFSET is 0x200 from global timer base
	 * the default clock source to the MPIC timer 0 is CCB freq / 8.
	 * to extend the timer period, we divide the timer clock source
	 * as CCB freq / 64, so the max timer period is 336 seconds
	 * when the CCB frequence is 400MHz.
	 */
	if (!has_tcr) {
		priv->ticks_per_sec = busfreq / 8;
	} else {
		u32 __iomem *tcr;
		tcr = (u32 __iomem *)((u32)priv->regs + MPIC_TIMER_TCR_OFFSET);
		out_be32(tcr, in_be32(tcr) | MPIC_TIMER_TCR_CLKDIV_64);
		priv->ticks_per_sec = busfreq / 64;
	}

	ret = device_create_file(&dev->dev, &dev_attr_timeout);
	if (ret)
		goto out;

	printk("MPIC global timer init done.\n");

	return 0;

out:
	kfree(priv);
	return ret;
}

static int __devexit mpic_tm_remove(struct of_device *dev)
{
	struct mpic_tm_priv *priv = dev_get_drvdata(&dev->dev);

	device_remove_file(&dev->dev, &dev_attr_timeout);
	free_irq(priv->irq, priv);
	iounmap(priv->regs);

	dev_set_drvdata(&dev->dev, NULL);
	kfree(priv);
	return 0;
}

static struct mpic_type mpic_types[] = {
	{
		.has_tcr = 0,
	},
	{
		.has_tcr = 1,
	}
};

static struct of_device_id mpic_tm_match[] = {
	{
		.compatible = "fsl,mpic-global-timer",
		.data = &mpic_types[1],
	},
	{},
};

static struct of_platform_driver mpic_tm_driver = {
	.name = "mpic-global-timer",
	.match_table = mpic_tm_match,
	.probe = mpic_tm_probe,
	.remove = __devexit_p(mpic_tm_remove)
};

static int __init mpic_tm_init(void)
{
	return of_register_platform_driver(&mpic_tm_driver);
}

static void __exit mpic_tm_exit(void)
{
	of_unregister_platform_driver(&mpic_tm_driver);
}

module_init(mpic_tm_init);
module_exit(mpic_tm_exit);
