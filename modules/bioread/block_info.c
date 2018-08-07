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

/* msdos file system structs */
#include <linux/msdos_fs.h>

/* reading from disk to memory */
#include <linux/buffer_head.h>

#define PERM	0000 /* parameter permissions */
#define MODE 	FMODE_READ

#define SECTOR 512

#define bdev_bread(bdev, block, size)\
	(__bread(bdev, block, size))

/* message passed in from user */
static char* dev;
static struct block_device* bdev;

/* set string to msg pointer */
module_param(dev, charp, PERM);
MODULE_PARM_DESC(dev, "Device path name");

static int blk_init(void)
{
	//struct buffer_head* bh;
	//struct fat_boot_sector* fbs;
	//char name[9];

	struct super_block* sb;

	bdev = blkdev_get_by_path(dev, MODE, NULL);

	if(IS_ERR_OR_NULL(bdev))
		return PTR_ERR(bdev);

	/* now try to obtain superblock ref. */
	sb = get_super(bdev);

	if(sb == NULL)
		goto fail;

	printk(KERN_INFO "Got Superblock\n");
	return 0;

	#if 0
	/* read first block from disk (boot sect)*/
	bh = bdev_bread(bdev, 0, SECTOR);
	if(bh == NULL)
	{
		printk(KERN_ALERT "Error: failed to read block");
		goto fail;
	}

	/* get fat boot sector reference */
	fbs = (struct fat_boot_sector*) bh->b_data;

	if(fbs == NULL)
	{
		brelse(bh);
		goto fail;
	}

	/* copy name for printing */
	strncpy(name, fbs->fat32.fs_type, 8);
	name[8] = 0;

	printk(KERN_INFO "FS Name: %s", name);

	/* release buffer head data */
	fbs = NULL;
	brelse(bh);

	return 0;
	#endif

	/* failed attempt */
fail:
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
MODULE_DESCRIPTION("Module looks at block device");