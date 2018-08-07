/* A collection of a trivial
 * set of functions implemented
 * due to non-exported symbols or
 * for kernel lack-of-need issues.
 */
#ifndef __TRIVIAL_H__
#define __TRIVIAL_H__

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/genhd.h>

/* type modifiers */
#define bdev_to_part(bdev) 	(bdev->bd_part)
#define dev_to_kobj(dev)	(&((dev)->kobj))

#define fs_name(fs) 		(fs->name)
#define kobj_name(kobj)		(kobj->name)

/* implementation: /fs/filsystems.c */
static inline struct file_system_type*
get_file_system(struct file_system_type* fs)
{
	__module_get(fs->owner);
	return fs;
}

/* implementation: /fs/filsystems.c */
static inline void
put_file_system(struct file_system_type* fs)
{
	module_put(fs->owner);
}

/* returns kobj or NULL */
static inline struct kobject*
bdev_to_kobj(struct block_device* bdev)
{
	struct kobject* kobj = NULL;
	struct device* dev;
	struct hd_struct* part;

	part = bdev_to_part(bdev);
	if(part == NULL)
		goto out;
	dev = part_to_dev(part);
	if(dev == NULL)
		goto out;
	kobj = dev_to_kobj(dev);
	kobject_get(kobj);

out:
	
	part = NULL;
	dev = NULL;

	return kobj;
}

#endif /* __TRIVIAL_H__ */