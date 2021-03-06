/*******************************************************************************
 *
 *  drivers/rtc/rtc-cns3xxx.c
 *
 *  Real Time Clock driver for the CNS3XXX SOCs
 *
 *  Author: Scott Shu
 *
 *  Copyright (c) 2008 Cavium Networks
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License, Version 2, as
 *  published by the Free Software Foundation.
 *
 *  This file is distributed in the hope that it will be useful,
 *  but AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
 *  NONINFRINGEMENT.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this file; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA or
 *  visit http://www.gnu.org/licenses/.
 *
 *  This file may also be available under a different license from Cavium.
 *  Contact Cavium Networks for more information
 *
 ******************************************************************************/

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/time.h>
#include <linux/rtc.h>
#include <linux/bcd.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <asm/irq.h>
#include <linux/uaccess.h>

#include <linux/mtd/mtd.h>

#include <mach/cns3xxx.h>

/* select system clock as the RTC reference clock */
#undef RTC_TEST

#define RTC_IS_OPEN			0x01	/* /dev/rtc is in use */
#define RTC_TIMER_ON			0x02

#define RTC_INTR_ALARM			0x20
#define MINUS_30PPM			0x0
#define MINUS_15PPM			0x1
#define MINUS_10PPM			0x2
#define MINUS_0PPM			0x3
#define PLUS_10PPM			0x4
#define PLUS_15PPM			0x5
#define PLUS_30PPM			0x6
#define DEFAULT_PPM			MINUS_0PPM

#define RTC_ENABLE			(1 << 0)
#define RTC_AUTO_ALARM_SEC_EN		(1 << 1)
#define RTC_AUTO_ALARM_MIN_EN		(1 << 2)
#define RTC_AUTO_ALARM_HOUR_EN		(1 << 3)
#define RTC_AUTO_ALARM_DAY_EN		(1 << 4)
#define RTC_MATCH_ALARM_INTC_EN		(1 << 5)
#define RTC_SYSTEM_CLK			(1 << 6)
#define RTC_ACCESS_CMD			(1 << 7)
#define RTC_DEFUALT_DIGI_TRIM		(DEFAULT_PPM << 21)
#define RTC_SOFT_RESET			(1 << 24)
#define RTC_INTR_STATUS_SEC		(1 << 0)
#define RTC_INTR_STATUS_MIN		(1 << 1)
#define RTC_INTR_STATUS_HOUR		(1 << 2)
#define RTC_INTR_STATUS_DAY		(1 << 3)
#define RTC_INTR_STATUS_ALARM		(1 << 4)

#ifdef RTC_DEBUG
#define __pr_debug(fmt, args...)		printk(KERN_ERR fmt, ##args)
#else
#define __pr_debug(fmt, args...)
#endif

/* power saving sate */
#define NO_MODE					0
#define STANDBY_MODE				1
#define MEM_MODE				2
#define DISK_MODE				3
#define ON_MODE					4

static struct resource *cns3xxx_rtc_mem;
static void __iomem *cns3xxx_rtc_base;
static int cns3xxx_rtc_alarmno = NO_IRQ;
static spinlock_t rtc_lock;

static unsigned long rtc_status;
static struct rtc_time set_alarm_tm_offset;
static struct rtc_time set_alarm_tm;
static int pm_state;
static struct device *device;

static int cns3xxx_rtc_gettime(struct device *dev, struct rtc_time *rtc_tm);
static int cns3xxx_rtc_getalarm(struct device *dev, struct rtc_wkalrm *alarm);

struct cns3xxx_rtc_parameter {
#define CNS3XXX_RTC_MAGIC	(0x20110601)
	u32 magic;
	unsigned long record;
};

static int cns3xxx_rtc_save_rec(unsigned long rtc_rec)
{
	struct cns3xxx_rtc_parameter cns_rtc;
	struct erase_info instr;
	int bytes = 0;
	int ret;
	struct mtd_info *mtd_rtc = get_mtd_device_nm("RTC Parameter");

	if ((struct mtd_info *) -ENODEV == mtd_rtc)
		return -ENODEV;

	memset(&instr, 0, sizeof(struct erase_info));
	instr.mtd = mtd_rtc;
	instr.addr = 0;
	instr.len = mtd_rtc->erasesize;

	ret = mtd_rtc->erase(mtd_rtc, &instr);
	if (ret < 0) {
		printk(KERN_ERR "mtd erase failed %d\n", ret);
		return ret;
	}

	cns_rtc.magic = CNS3XXX_RTC_MAGIC;
	cns_rtc.record = rtc_rec;
	ret = mtd_rtc->write(mtd_rtc, 0, sizeof(struct cns3xxx_rtc_parameter),
			 &bytes, (u_char *) &cns_rtc);

	return 0;
}

static int cns3xxx_rtc_read_rec(unsigned long *rtc_rec)
{
	struct cns3xxx_rtc_parameter cns_rtc;
	int bytes = 0;
	int ret;
	struct mtd_info *mtd_rtc = get_mtd_device_nm("RTC Parameter");

	if ((struct mtd_info *) -ENODEV == mtd_rtc)
		return -ENODEV;

	ret = mtd_rtc->read(mtd_rtc, 0, sizeof(struct cns3xxx_rtc_parameter),
			&bytes, (u_char *) &cns_rtc);
	if (bytes != sizeof(struct cns3xxx_rtc_parameter)) {
		printk(KERN_ERR "mtd read failed %d\n", bytes);
		return -EIO;
	}
	if (cns_rtc.magic != CNS3XXX_RTC_MAGIC) {
		printk(KERN_ERR "Unknown rtc magic %lx\n", cns_rtc.record);
		return -ENODEV;
	}

	*rtc_rec = cns_rtc.record;
	return 0;
}


static irqreturn_t cns3xxx_rtc_alarmirq(int irq, void *id)
{
	struct rtc_device *dev = id;
	struct rtc_time current_rtc_time;

	__pr_debug("alarm interrupt !\n");
	cns3xxx_rtc_gettime(device, &current_rtc_time);

	__pr_debug("pm_state=%d\n", pm_state);
	writeb(RTC_INTR_STATUS_ALARM, cns3xxx_rtc_base + RTC_INTR_STS_OFFSET);

	if (current_rtc_time.tm_mday == set_alarm_tm.tm_mday) {
		__pr_debug("tmday == alamday\n");
		rtc_update_irq(dev, 1, RTC_AF | RTC_IRQF);
	}
	return IRQ_HANDLED;
}

static int cns3xxx_rtc_open(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct rtc_device *rtc_dev = platform_get_drvdata(pdev);
	int ret = -EBUSY;

	if (rtc_status & RTC_IS_OPEN)
		goto out;

	rtc_status |= RTC_IS_OPEN;

	ret =
	    request_irq(cns3xxx_rtc_alarmno, cns3xxx_rtc_alarmirq,
			IRQF_DISABLED, "cns3xxx-rtc alarm", rtc_dev);

	if (ret) {
		dev_err(dev, "IRQ%d error %d\n", cns3xxx_rtc_alarmno, ret);
		goto out;
	}
	device = dev;
	ret = 0;
out:
	return ret;
}

static void cns3xxx_rtc_release(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct rtc_device *rtc_dev = platform_get_drvdata(pdev);

	free_irq(cns3xxx_rtc_alarmno, rtc_dev);

	rtc_status &= ~RTC_IS_OPEN;
}

static int cns3xxx_rtc_ioctl(struct device *dev, unsigned int cmd,
			     unsigned long arg)
{
	unsigned long ctrl;

	ctrl = readl(cns3xxx_rtc_base + RTC_CTRL_OFFSET);

	switch (cmd) {
	case RTC_AIE_OFF:
		__pr_debug("cns3xxx_rtc_ioctl: disable alarm\n");
		ctrl &= ~RTC_MATCH_ALARM_INTC_EN;
		writel(ctrl, cns3xxx_rtc_base + RTC_CTRL_OFFSET);
		return 0;
	case RTC_AIE_ON:
		__pr_debug("cns3xxx_rtc_ioctl: enable alarm\n");
		ctrl |= RTC_MATCH_ALARM_INTC_EN;
		writel(ctrl, cns3xxx_rtc_base + RTC_CTRL_OFFSET);
		return 0;
	default:
		__pr_debug("un support ioctl:%ux\n", cmd);
	}
	return -ENOIOCTLCMD;
}

static int cns3xxx_rtc_gettime(struct device *dev, struct rtc_time *rtc_tm)
{
	unsigned int second, minute, hour, day;
	unsigned long rtc_record;
	unsigned long total_second;

	spin_lock(&rtc_lock);

	second = readw(cns3xxx_rtc_base + RTC_SEC_OFFSET);
	minute = readw(cns3xxx_rtc_base + RTC_MIN_OFFSET);
	hour = readw(cns3xxx_rtc_base + RTC_HOUR_OFFSET);
	day = readw(cns3xxx_rtc_base + RTC_DAY_OFFSET);
	rtc_record = readl(cns3xxx_rtc_base + RTC_REC_OFFSET);

	spin_unlock(&rtc_lock);

	total_second =
	    day * 24 * 60 * 60 + hour * 60 * 60 + minute * 60 + second +
	    rtc_record;

	rtc_time_to_tm(total_second, rtc_tm);

	__pr_debug("read time %02x.%02x.%02x %02x/%02x/%02x\n",
		 rtc_tm->tm_year, rtc_tm->tm_mon, rtc_tm->tm_mday,
		 rtc_tm->tm_hour, rtc_tm->tm_min, rtc_tm->tm_sec);

	return 0;
}

static int cns3xxx_rtc_settime(struct device *dev, struct rtc_time *tm)
{
	unsigned int second, minute, hour, day;
	unsigned long rtc_record;

	__pr_debug("set time %02d.%02d.%02d %02d/%02d/%02d\n",
		 tm->tm_year, tm->tm_mon, tm->tm_mday,
		 tm->tm_hour, tm->tm_min, tm->tm_sec);

	rtc_tm_to_time(tm, &rtc_record);

	spin_lock(&rtc_lock);

	second = readw(cns3xxx_rtc_base + RTC_SEC_OFFSET);
	minute = readw(cns3xxx_rtc_base + RTC_MIN_OFFSET);
	hour = readw(cns3xxx_rtc_base + RTC_HOUR_OFFSET);
	day = readw(cns3xxx_rtc_base + RTC_DAY_OFFSET);

	rtc_record -=
	    day * 24 * 60 * 60 + hour * 60 * 60 + minute * 60 + second;
	writel(rtc_record, cns3xxx_rtc_base + RTC_REC_OFFSET);
	spin_unlock(&rtc_lock);

	if (cns3xxx_rtc_save_rec(rtc_record) < 0) {
		printk(KERN_ERR "write rtc_rec to flash failed!");
		return -EIO;
	}

	return 0;
}

static int cns3xxx_rtc_getalarm(struct device *dev, struct rtc_wkalrm *alarm)
{
	struct rtc_time *alm_tm = &alarm->time;
	unsigned long total_alarm_second;
	unsigned char status;
	unsigned long ctrl;

	spin_lock(&rtc_lock);

	alm_tm->tm_sec = readw(cns3xxx_rtc_base + RTC_SEC_ALM_OFFSET);
	alm_tm->tm_min = readw(cns3xxx_rtc_base + RTC_MIN_ALM_OFFSET);
	alm_tm->tm_hour = readw(cns3xxx_rtc_base + RTC_HOUR_ALM_OFFSET);

	if ((alm_tm->tm_sec != 0) ||
		(alm_tm->tm_min != 0) ||
		(alm_tm->tm_hour != 0)) {
		rtc_tm_to_time(&set_alarm_tm_offset, &total_alarm_second);
		total_alarm_second += readl(cns3xxx_rtc_base + RTC_REC_OFFSET);
		__pr_debug("get alarm total sec:%lux\n", total_alarm_second);
		rtc_time_to_tm(total_alarm_second, alm_tm);
	}

	status = readb(cns3xxx_rtc_base + RTC_INTR_STS_OFFSET);
	alarm->pending = (status & RTC_INTR_STATUS_ALARM) ? 1 : 0;

	ctrl = readl(cns3xxx_rtc_base + RTC_CTRL_OFFSET);
	alarm->enabled = (ctrl & RTC_MATCH_ALARM_INTC_EN) ? 1 : 0;

	spin_unlock(&rtc_lock);

	__pr_debug("cns3xxx_rtc_getalarm: (%d),"
		"%02x/%02x/%02x-%02x.%02x.%02x\n",
		 alarm->enabled,
		 alm_tm->tm_year, alm_tm->tm_mon, alm_tm->tm_mday,
		 alm_tm->tm_hour, alm_tm->tm_min, alm_tm->tm_sec);

	return 0;
}

static int cns3xxx_rtc_setalarm(struct device *dev, struct rtc_wkalrm *alarm)
{
	struct rtc_time *alm_tm = &alarm->time;
	unsigned long ctrl;
	unsigned long total_alarm_second;

	__pr_debug("cns3xxx_rtc_setalarm: (%d),"
		"%02x/%02x/%02x %02x.%02x.%02x\n",
		 alarm->enabled,
		 alm_tm->tm_year & 0xff, alm_tm->tm_mon & 0xff,
		 alm_tm->tm_mday & 0xff, alm_tm->tm_hour & 0xff,
		 alm_tm->tm_min & 0xff, alm_tm->tm_sec);

	spin_lock(&rtc_lock);
	memcpy(&set_alarm_tm, alm_tm, sizeof(struct rtc_time));
	rtc_tm_to_time(alm_tm, &total_alarm_second);

	total_alarm_second -= readl(cns3xxx_rtc_base + RTC_REC_OFFSET);
	__pr_debug("alarm time (second): %lux\n", total_alarm_second);

	rtc_time_to_tm(total_alarm_second, &set_alarm_tm_offset);

	__pr_debug("alarm time ():%02x,%02x/%02x/%02x\n",
		set_alarm_tm_offset.tm_mday, set_alarm_tm_offset.tm_hour,
		set_alarm_tm_offset.tm_min, set_alarm_tm_offset.tm_sec);
	writeb(set_alarm_tm_offset.tm_sec,
		cns3xxx_rtc_base + RTC_SEC_ALM_OFFSET);
	writeb(set_alarm_tm_offset.tm_min,
		cns3xxx_rtc_base + RTC_MIN_ALM_OFFSET);
	writeb(set_alarm_tm_offset.tm_hour,
		cns3xxx_rtc_base + RTC_HOUR_ALM_OFFSET);

	__pr_debug("cns3xxx_rtc_setalarm 2: %02x.%02x.%02x\n",
			set_alarm_tm_offset.tm_hour,
			set_alarm_tm_offset.tm_min,
			set_alarm_tm_offset.tm_sec);

	writeb(RTC_INTR_STATUS_ALARM, cns3xxx_rtc_base + RTC_INTR_STS_OFFSET);
	alarm->pending = 0;

	ctrl = readl(cns3xxx_rtc_base + RTC_CTRL_OFFSET);
	writel(ctrl | RTC_MATCH_ALARM_INTC_EN,
	       cns3xxx_rtc_base + RTC_CTRL_OFFSET);
	alarm->enabled = 1;
	pm_state = ON_MODE;
	spin_unlock(&rtc_lock);

	if (alarm->enabled)
		enable_irq_wake(cns3xxx_rtc_alarmno);
	else
		disable_irq_wake(cns3xxx_rtc_alarmno);
	return 0;
}

static int cns3xxx_rtc_proc(struct device *dev, struct seq_file *seq)
{
	return 0;
}

static int cns3xxx_periodic_irq_set_state(struct device *dev, int enabled)
{
	__pr_debug("%s,%s,%d\n", __FILE__, __func__, __LINE__);
	return 0;
}

static int cns3xxx_periodic_irq_set_freq(struct device *dev, int freq)
{
	__pr_debug("%s,%s,%d\n", __FILE__, __func__, __LINE__);
	return 0;
}

static const struct rtc_class_ops cns3xxx_rtcops = {
	.open		= cns3xxx_rtc_open,
	.release	= cns3xxx_rtc_release,
	.ioctl		= cns3xxx_rtc_ioctl,
	.read_time	= cns3xxx_rtc_gettime,
	.set_time	= cns3xxx_rtc_settime,
	.read_alarm	= cns3xxx_rtc_getalarm,
	.set_alarm	= cns3xxx_rtc_setalarm,
	.proc		= cns3xxx_rtc_proc,
	.irq_set_state = cns3xxx_periodic_irq_set_state,
	.irq_set_freq	= cns3xxx_periodic_irq_set_freq,
};

static int cns3xxx_rtc_probe(struct platform_device *dev)
{
	struct rtc_device *rtc;
	struct resource *res;
	unsigned long ctrl;
	unsigned long rtc_rec;

	int ret = 0;

	printk(KERN_NOTICE "%s: probe=%p\n", __func__, dev);

	/* We only accept one device, and it must have an id of -1 */
	if (dev->id != -1)
		return -ENODEV;

	cns3xxx_rtc_alarmno = platform_get_irq(dev, 0);
	if (cns3xxx_rtc_alarmno < 0) {
		dev_err(&dev->dev, "no irq for alarm\n");
		return -ENOENT;
	}
	__pr_debug("cns3xxx_rtc: alarm irq %d\n", cns3xxx_rtc_alarmno);

	res = platform_get_resource(dev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		dev_err(&dev->dev, "failed to get memory region resource\n");
		return -ENODEV;
	}
	cns3xxx_rtc_mem =
	    request_mem_region(res->start, res->end - res->start + 1,
			       dev->name);
	if (cns3xxx_rtc_mem == NULL) {
		dev_err(&dev->dev, "failed to reserve memory region\n");
		ret = -ENOENT;
		goto err_nortc;
	}

	cns3xxx_rtc_base = ioremap(res->start, res->end - res->start + 1);
	if (cns3xxx_rtc_base == NULL) {
		dev_err(&dev->dev, "failed ioremap()\n");
		ret = -ENOMEM;
		goto err_free;
	}

	/* first, disable RTC and initial RTC alarm registers */
	writel(0, cns3xxx_rtc_base + RTC_CTRL_OFFSET);
	writel(0, cns3xxx_rtc_base + RTC_SEC_ALM_OFFSET);
	writel(0, cns3xxx_rtc_base + RTC_MIN_ALM_OFFSET);
	writel(0, cns3xxx_rtc_base + RTC_HOUR_ALM_OFFSET);

	/* enable RTC */
	ctrl = readl(cns3xxx_rtc_base + RTC_CTRL_OFFSET);
#ifdef RTC_TEST
	/* select system clock as the RTC reference clock */
	writel(ctrl | RTC_ENABLE | RTC_SYSTEM_CLK | RTC_DEFUALT_DIGI_TRIM,
	       cns3xxx_rtc_base + RTC_CTRL_OFFSET);
#else
	writel(ctrl | RTC_ENABLE | RTC_ACCESS_CMD | RTC_DEFUALT_DIGI_TRIM,
		cns3xxx_rtc_base + RTC_CTRL_OFFSET);
#endif

	/* register RTC */
	rtc =
	    rtc_device_register("cns3xxx-rtc", &dev->dev, &cns3xxx_rtcops,
				THIS_MODULE);
	if (IS_ERR(rtc)) {
		dev_err(&dev->dev, "cannot attach rtc\n");
		ret = PTR_ERR(rtc);
		goto err_nores;
	}

	/* since RTC can't save the hardware time.
	 * we need to retrieve the offset rec from flash.
	 */
	if (cns3xxx_rtc_read_rec(&rtc_rec) == 0)
		writel(rtc_rec, cns3xxx_rtc_base + RTC_REC_OFFSET);

	device_init_wakeup(&dev->dev, 1);
	platform_set_drvdata(dev, rtc);

	return 0;

err_nores:
	iounmap(cns3xxx_rtc_base);

err_free:
	release_resource(cns3xxx_rtc_mem);
	kfree(cns3xxx_rtc_mem);
err_nortc:
	__pr_debug("probe func has something wrong!");
	return ret;
}

static int cns3xxx_rtc_remove(struct platform_device *dev)
{
	struct rtc_device *rtc = platform_get_drvdata(dev);

	__pr_debug("%s: remove=%p\n", __func__, dev);

	platform_set_drvdata(dev, NULL);

	rtc_device_unregister(rtc);

	iounmap(cns3xxx_rtc_base);
	release_resource(cns3xxx_rtc_mem);
	kfree(cns3xxx_rtc_mem);

	return 0;
}

#ifdef CONFIG_PM

static int cns3xxx_rtc_suspend(struct platform_device *dev, pm_message_t state)
{
	__pr_debug("%s,%s,%d\n", __FILE__, __func__, __LINE__);
	return 0;
}

static int cns3xxx_rtc_resume(struct platform_device *dev)
{
	__pr_debug("%s,%s,%d\n", __FILE__, __func__, __LINE__);

	return 0;
}

#else
#define cns3xxx_rtc_suspend	NULL
#define cns3xxx_rtc_resume	NULL
#endif /* CONFIG_PM */

static struct platform_driver cns3xxx_rtcdrv = {
	.probe = cns3xxx_rtc_probe,
	.remove = __devexit_p(cns3xxx_rtc_remove),
	.suspend =	cns3xxx_rtc_suspend,
	.resume = cns3xxx_rtc_resume,
	.driver = {
		.name	= "cns3xxx-rtc",
		.owner	= THIS_MODULE,
	},
};

static char banner[] __initdata =
    KERN_INFO "CNS3XXX Real Time Clock, (c) 2009 Cavium Networks\n";

static int __init cns3xxx_rtc_init(void)
{
	printk(banner);

	spin_lock_init(&rtc_lock);

	return platform_driver_register(&cns3xxx_rtcdrv);
}

static void __exit cns3xxx_rtc_exit(void)
{
	platform_driver_unregister(&cns3xxx_rtcdrv);
}

module_init(cns3xxx_rtc_init);
module_exit(cns3xxx_rtc_exit);

MODULE_AUTHOR("Scott Shu");
MODULE_DESCRIPTION("Cavium Networks CNS3XXX RTC Driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:cns3xxx-rtc");
