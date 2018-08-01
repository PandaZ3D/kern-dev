/* 
 * Allen Aboytes 
 * 8/1/2018
 *
 * sysfs_file.c - creates a file under /sys/fs/free
 *
 */
 
/* standard headers for kernel modules */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>
#include <linux/fs.h> /* fs_kobj */

#define NAME 		"newfile"	/* file name */
#define PERM 		0444 		/* file permissions */
#define PARENT 		NULL

#define FILE_ATTR_RO(_name)\
	struct kobj_attribute kobj_attr_##_name = __ATTR_RO(_name)

static struct kobject* new_kobj;
static int special = 0xfeed;

/*
 * sysfs allocates a buffer of size (PAGE_SIZE)
 * On read(2) method should fill the entire buffer.
 * return the number of bytes, return value of scnprintf()
 */
static ssize_t 
newfile_show(struct kobject *kobj, struct kobj_attribute *attr, 
	char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "0x%X\n", special);
}

/*
 * returns the number of bytes used from the buffer. If the
 * entire buffer has been used, just return the count argument.
 */
/*
static ssize_t
newfile_store(struct kobject *kobj, struct kobj_attribute *attr,
	const char *buf, size_t count)
{
	return 0;
}
*/

/* kobject attributes for file */
static FILE_ATTR_RO(newfile);

/* 
 * RETURN: 0 if successfully loaded, nonzero else
 */
static int newfile_init(void)
{
	int ret = 0;
	
	/* create new kobject for use in sysfs (a new directory) */
	//new_kobj = kobject_create_and_add("newfolder", PARENT);
	
	/* not enough memory to allocate */
	//if(new_kobj == NULL)
	//{
	//	printk(KERN_ALERT "Error: on creating sysfs file entry.\n");
	//	return -ENOMEM;
	//}

	/* try to create new sysfs entry (file) */
	//ret = sysfs_create_file(fs_kobj, &kobj_attr_newfile.attr);

	/* log error message */
	//if(ret)
		//printk(KERN_ALERT "Error: on creating sysfs file entry.\n");
	sysfs_remove_file(fs_kobj, &kobj_attr_newfile.attr);
	
	return 0;
}

/* 
 * 
 */
static void newfile_exit(void)
{
	/* remove instance of kernel object */
	//kobject_put(new_kobj);
	//sysfs_remove_file(fs_kobj, &kobj_attr_newfile.attr);
}
 
/* register module start and end functions */
module_init(newfile_init);
module_exit(newfile_exit);
 
/* basic description of module */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zed");
MODULE_DESCRIPTION("Simple Module to create file in /sys");