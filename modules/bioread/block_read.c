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
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/fs.h>

/* file system includes */
#include <linux/msdos_fs.h>

/* private declarations */
//#include <fat.h>

#include "trivial.h"

#define PERM	0000 /* parameter permissions */
#define MODE 	FMODE_READ
#define MAX		1024

/* device path specified from module */
static char* path;

/* block device reference */
static struct block_device* bdev;

/* set string to path pointer */
module_param(path, charp, PERM);
MODULE_PARM_DESC(path, "Device path name");

static int blk_init(void)
{
	struct super_block* sb = NULL;
	struct file_system_type* fst = NULL;
	const struct super_operations* ops;

	int i = 0;
	/***************************************************************/
	/* 						  TEST CODE							   */
	/***************************************************************/
	/* now get the first five FAT entries */
	//char buffer[MAX];
	#if 1
	struct msdos_sb_info *sbi;
	const struct fatent_operations *fops;
	struct fat_entry fatent;
	unsigned long reada_blocks, reada_mask, cur_block;
	int err = 0, c = FAT_ENT_EOF;//, p = FAT_ENT_EOF;
	#endif
	/***************************************************************/
	/*						TEST CODE END 						   */
	/***************************************************************/

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

	/***************************************************************/
	/* 						  TEST CODE							   */
	/***************************************************************/
	#if 1
	sbi = MSDOS_SB(sb);
	fops = sbi->fatent_ops;

	/* lock reference to sb */
	lock_fat(sbi);
	if (sbi->free_clusters != -1 && sbi->free_clus_valid)
		goto out;
	
	reada_blocks = FAT_READA_SIZE >> sb->s_blocksize_bits; //number of blocks to read
	reada_mask = reada_blocks - 1; //reading mask
	cur_block = 0;

	fatent_init(&fatent);
	fatent_set_entry(&fatent, FAT_START_ENT);
	
	/* loop start */
	while(i < 4)
	{
		if ((cur_block & reada_mask) == 0) {
			unsigned long rest = sbi->fat_length - cur_block;
			fat_ent_reada(sb, &fatent, min(reada_blocks, rest));
		}
		cur_block++;

		err = fat_ent_read_block(sb, &fatent);
		if (err)
			goto out;
		do
		{
			c = fops->ent_get(&fatent);
			
			printk(KERN_INFO "%d:%x\n", fatent.entry, c);
		} while (i < 4);
	}
	/* loop end */

	fatent_brelse(&fatent);
	
	unlock_fat(sbi);
	
	#endif	
	/***************************************************************/
	/*						TEST CODE END 						   */
	/***************************************************************/

	/* clean up and dec ref counts */
	ops->put_super(sb);
	put_file_system(fst);

	return 0;
out:
	unlock_fat(sbi);
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