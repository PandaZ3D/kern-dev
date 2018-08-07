/* 
 * Allen Aboytes 
 * 8/1/2018
 *
 * block_info.c - prints device info
 *
 * Externel module taints kernel
 */
 
/* standard headers for kernel modules */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

/* file system includes */
#include <linux/fs.h>
#include <linux/genhd.h>

#define PERM	0000 /* parameter permissions */
#define MODE 	FMODE_READ

/* message passed in from user */
static char* dev;
static struct block_device* bdev;

/* set string to msg pointer */
module_param(dev, charp, PERM);
MODULE_PARM_DESC(dev, "Device path name");

static int blk_init(void)
{
	struct hd_struct* part;

	bdev = blkdev_get_by_path(dev, MODE, NULL);

	if(IS_ERR_OR_NULL(bdev))
		return PTR_ERR(bdev);

	/* successful opening */
	//printk(KERN_INFO "Success: opened: %s\n", dev);

	/* we are working with sdb */
	part = bdev->bd_part;

	if(part == NULL)
	{
		printk(KERN_ALERT "Unable to obtain partition.");
		blkdev_put(bdev, MODE);
	}

	printk(KERN_INFO "Device name:%s Disk name:%s, Maj:%d, Min:%d", 
	dev_name(&part->__dev),
	bdev->bd_disk->disk_name,
	bdev->bd_disk->major,
	part->partno);

	return 0;
}

/*  
 * blkdev_exit() - called when module is removed
 */
static void blk_exit(void)
{
	printk(KERN_INFO "Graceful Exit.\n");
	blkdev_put(bdev, MODE);
}
 
/* register module start and end functions */
module_init(blk_init);
module_exit(blk_exit);
 
/* basic description of module */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zed");
MODULE_DESCRIPTION("Module looks at block device");