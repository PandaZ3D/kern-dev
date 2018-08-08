/* A collection of a trivial
 * set of functions implemented
 * due to non-exported symbols.
 */
#ifndef __TRIVIAL_H__
#define __TRIVIAL_H__

#include <linux/fs.h>
#include <linux/module.h>

#include "fatfs.h"

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

/* returns file system name */
static inline const char*
get_fs_name(struct file_system_type* fs)
{
	return fs->name;
}

#endif /* __TRIVIAL_H__ */