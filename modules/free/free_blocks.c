/* 
 * Allen Aboytes 
 * 8/1/2018
 *
 * free_blocks.c - creates a file under /sys/fs/free
 * for a specific block device
 *
 */
 
/* standard headers for kernel modules */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>
#include <linux/fs.h> /* fs_kobj */

#include "trivial.h"

#define NAME 	"free"		/* directory name */
#define PARENT 	fs_kobj
#define MODE 	FMODE_READ
#define PERM	0000 		/* parameter permissions */

#define FILE_ATTR_RO(_name)\
	struct kobj_attribute kobj_attr_##_name = __ATTR_RO(_name)

static int special = 0x80085;
static char* path;

module_param(path, charp, PERM);
MODULE_PARM_DESC(path, "Device path name");


static struct block_device* bdev;
static struct kobject *free_kobj, *bd_kobj;

/*
 * sysfs allocates a buffer of size (PAGE_SIZE)
 * On read(2) method should fill the entire buffer.
 * return the number of bytes, return value of scnprintf()
 */
static ssize_t 
free_show(struct kobject *kobj, struct kobj_attribute *attr, 
	char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "0x%X\n", special);
}

/* kobject attributes for file */
static FILE_ATTR_RO(free);

/* 
 * RETURN: 0 if successfully loaded, nonzero else
 */
static int free_init(void)
{
	int ret = 0;
	
	/* create new kobject for use in sysfs (a new directory) */
	free_kobj = kobject_create_and_add(NAME, PARENT);
	
	/* not enough memory to allocate */
	if(free_kobj == NULL)
	{
		printk(KERN_ALERT "Error: on creating sysfs directory entry.\n");
		return -ENOMEM;
	}

	/* obtain block device reference */
	bdev = blkdev_get_by_path(path, MODE, NULL);

	if(IS_ERR_OR_NULL(bdev))
	{
		ret = PTR_ERR(bdev);
		goto fail;
	}

	/* get kobject reference for block device */
	bd_kobj = bdev_to_kobj(bdev);
	if(bd_kobj == NULL)
	{
		ret = -ENODEV;
		goto fail;
	}

	/* add kobject to sysfs */
	ret = kobject_add(bd_kobj, free_kobj, "%s", kobj_name(bd_kobj));
	if(ret)
		goto fail;

	/* try to create new sysfs entry (file) */
	ret = sysfs_create_file(bd_kobj, &kobj_attr_free.attr);

	/* log error message */
	if(ret)
		goto fail;

	return 0;

fail: 

	if(free_kobj)
		kobject_put(free_kobj);
	if(bd_kobj)
		kobject_put(bd_kobj);
	if(bdev)
		blkdev_put(bdev, MODE);

	return ret;
}

static void free_exit(void)
{
	/* remove file from with kobj */
	sysfs_remove_file(bd_kobj, &kobj_attr_free.attr);

	/* remove instance of kernel object */
	kobject_del(free_kobj);

	/* decrement reference count */
	kobject_put(bd_kobj);
	blkdev_put(bdev, MODE);
}
 
/* register module start and end functions */
module_init(free_init)
module_exit(free_exit)
 
/* basic description of module */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zed");
MODULE_DESCRIPTION("Finds free blocks in a filesystem");