/* 
 * Allen Aboytes 
 * 8/1/2018
 *
 * block_read.c - reads data from a device
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

/* magic number */
#include <linux/magic.h>

#define PERM	0000 /* parameter permissions */
#define MODE 	FMODE_READ

/* device path specified from module */
static char* path;

/* block device reference */
static struct block_device* bdev;

/* set string to path pointer */
module_param(path, charp, PERM);
MODULE_PARM_DESC(path, "Device path name");

static int blk_init(void)
{
	struct hd_struct* part;
	struct super_block* sb;

	/* obtain block device reference */
	bdev = blkdev_get_by_path(path, MODE, NULL);

	if(IS_ERR_OR_NULL(bdev))
		return PTR_ERR(bdev);

	/* get partition reference */
	part = bdev->bd_part;

	if(part == NULL)
	{
		printk(KERN_ALERT "Unable to obtain partition.");
		blkdev_put(bdev, MODE);
		return 0;
	}


	/* perform some sort of disk io */
	sb = bdev->bd_super;
	if(sb == NULL)
	{
		blkdev_put(bdev, MODE);
		return 0;
	}

	printk(KERN_INFO "Super Magic: 0x%lX : %c",
		sb->s_magic, (sb->s_magic & MSDOS_SUPER_MAGIC)? 'y':'n');

	part = NULL;
	sb = NULL;

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
module_init(blk_init)
module_exit(blk_exit)
 
/* basic description of module */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zed");
MODULE_DESCRIPTION("Module looks at block device");