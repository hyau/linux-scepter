/*
 * drivers/char/watchdog/cortexa9-wdt.c
 *
 * Watchdog driver for watchdog module present with ARM CORTEX A9
 *
 * Copyright (C) 2010 ST Microelectronics
 * Viresh Kumar<viresh.kumar@st.com>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <linux/bitops.h>
#include <linux/clk.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/kernel.h>
#include <linux/math64.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/watchdog.h>

/* default timeout in seconds */
#define DEFAULT_TIMEOUT		60

#define MODULE_NAME		"cortexa9-wdt"

/* watchdog register offsets and masks */
#define WDTLOAD			0x000
	#define LOAD_MIN	0x00000000
	#define LOAD_MAX	0xFFFFFFFF
#define WDTCOUNTER		0x004
#define WDTCONTROL		0x008
	/* control register masks */
	#define WDT_ENB		(1 << 0)
	#define WDT_ARLD_ENB	(1 << 1)	/* Autoreload enable */
	#define WDT_IRQ_ENB	(1 << 2)
	#define WDT_MODE	(1 << 3)
	#define WDT_PRESCALE(x)	((x) << 8)
	#define PRESCALE_MIN	0x00
	#define PRESCALE_MAX	0xFF
#define WDTINTSTS		0x00C
	#define INT_MASK	0x1
#define WDTRSTSTS		0x010
	#define RST_MASK	0x1
#define WDTDISABLE		0x014
	#define DISABLE_VAL1	0x12345678
	#define DISABLE_VAL2	0x87654321

/**
 * struct cortexa9_wdt: cortex a9 wdt device structure
 *
 * lock: spin lock protecting dev structure and io access
 * base: base address of wdt
 * clk: clock structure of wdt
 * pdev: platform device structure of wdt
 * boot_status: boot time status of wdt
 * status: current status of wdt
 * prescale: prescale value to be set for current timeout
 * load_val: load value to be set for current timeout
 * timeout: current programmed timeout
 */
struct cortexa9_wdt {
	spinlock_t			lock;
	void __iomem			*base;
	struct clk			*clk;
	struct platform_device		*pdev;
	unsigned long			boot_status;
	unsigned long			status;
	#define WDT_BUSY		0
	#define WDT_CAN_BE_CLOSED	1
	unsigned int			prescale;
	unsigned int			load_val;
	unsigned int			timeout;
};

/* local variables */
static struct cortexa9_wdt *wdt;
static int nowayout = WATCHDOG_NOWAYOUT;

/* binary search */
static inline void bsearch(u32 *var, u32 var_start, u32 var_end,
		const u64 param, const u32 timeout, u32 rate)
{
	u64 tmp = 0;

	/* get the lowest var value that can satisfy our requirement */
	while (var_start < var_end) {
		tmp = var_start;
		tmp += var_end;
		tmp = div_u64(tmp, 2);
		if (timeout > div_u64((param + 1) * (tmp + 1), rate)) {
			if (var_start == tmp)
				break;
			else
				var_start = tmp;
		} else {
			if (var_end == tmp)
				break;
			else
				var_end = tmp;
		}
	}
	*var = tmp;
}

/*
 * This routine finds the most appropriate prescale and load value for input
 * timout value
 */
static void wdt_config(unsigned int timeout)
{
	unsigned int psc, rate;
	u64 load = 0;

	rate = clk_get_rate(wdt->clk);

	/* get appropriate value of psc and load */
	bsearch(&psc, PRESCALE_MIN, PRESCALE_MAX, LOAD_MAX, timeout, rate);
	bsearch((u32 *)&load, LOAD_MIN, LOAD_MAX, psc, timeout, rate);

	spin_lock(&wdt->lock);
	wdt->prescale = psc;
	wdt->load_val = load;

	/* roundup timeout to closest positive integer value */
	wdt->timeout = div_u64((psc + 1) * (load + 1) + (rate / 2), rate);
	spin_unlock(&wdt->lock);
}

static void wdt_enable(void)
{
	u32 val;

	spin_lock(&wdt->lock);

	writel(wdt->load_val, wdt->base + WDTLOAD);
	val = WDT_MODE | WDT_PRESCALE(wdt->prescale) | WDT_ENB;
	writel(val, wdt->base + WDTCONTROL);

	spin_unlock(&wdt->lock);
}

static void wdt_disable(void)
{
	spin_lock(&wdt->lock);

	/* firstly we need to put watchdog in timer mode */
	writel(DISABLE_VAL1, wdt->base + WDTDISABLE);
	writel(DISABLE_VAL2, wdt->base + WDTDISABLE);

	writel(0, wdt->base + WDTCONTROL);
	writel(INT_MASK, wdt->base + WDTINTSTS);
	writel(RST_MASK, wdt->base + WDTRSTSTS);

	spin_unlock(&wdt->lock);
}

static ssize_t cortexa9_wdt_write(struct file *file, const char *data,
		size_t len, loff_t *ppos)
{
	if (len) {
		if (!nowayout) {
			size_t i;

			clear_bit(WDT_CAN_BE_CLOSED, &wdt->status);

			for (i = 0; i != len; i++) {
				char c;

				if (get_user(c, data + i))
					return -EFAULT;
				if (c == 'V') {
					set_bit(WDT_CAN_BE_CLOSED,
							&wdt->status);
					break;
				}
			}
		}
		wdt_enable();
	}
	return len;
}

static const struct watchdog_info ident = {
	.options = WDIOF_CARDRESET | WDIOF_MAGICCLOSE |
		WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING,
	.identity = MODULE_NAME,
};

static long cortexa9_wdt_ioctl(struct file *file, unsigned int cmd,
		unsigned long arg)
{
	int ret = -ENOTTY;
	int timeout;

	switch (cmd) {
	case WDIOC_GETSUPPORT:
		ret = copy_to_user((struct watchdog_info *)arg, &ident,
				sizeof(ident)) ? -EFAULT : 0;
		break;

	case WDIOC_GETSTATUS:
		ret = put_user(0, (int *)arg);
		break;

	case WDIOC_GETBOOTSTATUS:
		ret = put_user(wdt->boot_status, (int *)arg);
		break;

	case WDIOC_KEEPALIVE:
		wdt_enable();
		ret = 0;
		break;

	case WDIOC_SETTIMEOUT:
		ret = get_user(timeout, (int *)arg);
		if (ret)
			break;

		wdt_config(timeout);

		wdt_enable();
		/* Fall through */

	case WDIOC_GETTIMEOUT:
		ret = put_user(wdt->timeout, (int *)arg);
		break;
	}
	return ret;
}

static int cortexa9_wdt_open(struct inode *inode, struct file *file)
{
	int ret = 0;

	if (test_and_set_bit(WDT_BUSY, &wdt->status))
		return -EBUSY;

	ret = clk_enable(wdt->clk);
	if (ret) {
		dev_err(&wdt->pdev->dev, "clock enable fail");
		goto err;
	}

	wdt_enable();

	/* can not be closed, once enabled */
	clear_bit(WDT_CAN_BE_CLOSED, &wdt->status);
	return nonseekable_open(inode, file);

err:
	clear_bit(WDT_BUSY, &wdt->status);
	return ret;
}

static int cortexa9_wdt_release(struct inode *inode, struct file *file)
{
	if (!test_bit(WDT_CAN_BE_CLOSED, &wdt->status)) {
		clear_bit(WDT_BUSY, &wdt->status);
		dev_warn(&wdt->pdev->dev, "Device closed unexpectedly\n");
		return 0;
	}

	wdt_disable();
	clk_disable(wdt->clk);
	clear_bit(WDT_BUSY, &wdt->status);

	return 0;
}

static const struct file_operations cortexa9_wdt_fops = {
	.owner = THIS_MODULE,
	.llseek = no_llseek,
	.write = cortexa9_wdt_write,
	.unlocked_ioctl = cortexa9_wdt_ioctl,
	.open = cortexa9_wdt_open,
	.release = cortexa9_wdt_release,
};

static struct miscdevice cortexa9_wdt_miscdev = {
	.minor = WATCHDOG_MINOR,
	.name = "watchdog",
	.fops = &cortexa9_wdt_fops,
};

static int __devinit cortexa9_wdt_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct resource *res;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		ret = -ENOENT;
		dev_warn(&pdev->dev, "Memory resource not defined\n");
		goto err;
	}

	if (!request_mem_region(res->start, resource_size(res), pdev->name)) {
		dev_warn(&pdev->dev, "Failed to get memory region resource\n");
		ret = -ENOENT;
		goto err;
	}

	wdt = kzalloc(sizeof(*wdt), GFP_KERNEL);
	if (!wdt) {
		dev_warn(&pdev->dev, "Kzalloc failed\n");
		ret = -ENOMEM;
		goto err_kzalloc;
	}

	wdt->clk = clk_get(&pdev->dev, NULL);
	if (IS_ERR(wdt->clk)) {
		dev_warn(&pdev->dev, "Clock not found\n");
		ret = PTR_ERR(wdt->clk);
		goto err_clk_get;
	}

	wdt->base = ioremap(res->start, resource_size(res));
	if (!wdt->base) {
		ret = -ENOMEM;
		dev_warn(&pdev->dev, "ioremap fail\n");
		goto err_ioremap;
	}

	spin_lock_init(&wdt->lock);
	/* This checks if system booted after watchdog reset or not */
	ret = clk_enable(wdt->clk);
	if (!ret) {
		wdt->boot_status = (readl(wdt->base + WDTRSTSTS) & RST_MASK) ?
			WDIOF_CARDRESET : 0;
		wdt_disable();
		clk_disable(wdt->clk);
	}

	wdt->pdev = pdev;
	wdt_config(DEFAULT_TIMEOUT);

	ret = misc_register(&cortexa9_wdt_miscdev);
	if (ret < 0) {
		dev_warn(&pdev->dev, "cannot register misc device\n");
		goto err_misc_register;
	}

	dev_info(&pdev->dev, "registration successful\n");
	return 0;

err_misc_register:
	iounmap(wdt->base);
err_ioremap:
	clk_put(wdt->clk);
err_clk_get:
	kfree(wdt);
	wdt = NULL;
err_kzalloc:
	release_mem_region(res->start, resource_size(res));
err:
	dev_err(&pdev->dev, "Probe Failed!!!\n");
	return ret;
}

static int __devexit cortexa9_wdt_remove(struct platform_device *pdev)
{
	struct resource *res;

	misc_deregister(&cortexa9_wdt_miscdev);
	iounmap(wdt->base);
	clk_put(wdt->clk);
	kfree(wdt);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res)
		release_mem_region(res->start, resource_size(res));

	return 0;
}

static struct platform_driver cortexa9_wdt_driver = {
	.driver = {
		.name = MODULE_NAME,
		.owner	= THIS_MODULE,
	},
	.probe = cortexa9_wdt_probe,
	.remove = __devexit_p(cortexa9_wdt_remove),
};

static int __init cortexa9_wdt_init(void)
{
	return platform_driver_register(&cortexa9_wdt_driver);
}
module_init(cortexa9_wdt_init);

static void __exit cortexa9_wdt_exit(void)
{
	platform_driver_unregister(&cortexa9_wdt_driver);
}
module_exit(cortexa9_wdt_exit);

module_param(nowayout, int, 0);
MODULE_PARM_DESC(nowayout,
		"Set to 1 to keep watchdog running after device release");

MODULE_AUTHOR("Viresh Kumar <viresh.kumar@st.com>");
MODULE_DESCRIPTION("ARM CORTEX A9 Watchdog Driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS_MISCDEV(WATCHDOG_MINOR);
