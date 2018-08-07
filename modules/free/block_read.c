/* 
 * Allen Aboytes 
 * 8/1/2018
 *
 * block_read.c - prints fs on device
 *
 * Externel module taints kernel
 */
 
/* standard headers for kernel modules */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

/* file system includes */
#include <linux/fs.h>

#include "trivial.h"

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
	struct super_block* sb;
	struct file_system_type* fst;
	const struct super_operations* ops;

	/* obtain block device reference */
	bdev = blkdev_get_by_path(path, MODE, NULL);

	if(IS_ERR_OR_NULL(bdev))
		return PTR_ERR(bdev);

	/* get superblock reference */
	sb = get_super(bdev);
	if(sb == NULL)
	{
		goto fail;
	}

	ops = sb->s_op;

	/* obtain file system */
	fst = get_file_system(sb->s_type);
	if(fst == NULL)
	{
		goto fail;
	}

	printk(KERN_INFO "The disk file system: %s", get_fs_name(fst));

	/* clean up and dec ref counts */
	ops->put_super(sb);
	put_file_system(fst);

	return 0;

fail:
	if(sb)
		ops->put_super(sb);
	if(fst)
		put_file_system(fst);
	blkdev_put(bdev, MODE);
	return -ENODEV;

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
MODULE_DESCRIPTION("Module prints file system name");