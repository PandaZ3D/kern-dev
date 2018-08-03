/* 
 * Allen Aboytes 
 * 8/1/2018
 *
 * block_info.c - prints device info
 *
 * Module taints kernel on build
 */
 
/* standard headers for kernel modules */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#define PERM	0000 /* parameter permissions */
#define MAX		64	 /* max number of arguments */

/* message passed in from user */
static char* dev;

/* set string to msg pointer */
module_param(dev, charp, PERM);
MODULE_PARM_DESC(dev, "Device path name");

 /* 
  * hello_init() - called when module is loaded
  * RETURN: 0 if successfully loaded, nonzero else
  */
static int hello_init(void)
{
	printk(KERN_INFO "Device to be opened: %s\n", dev);
	return 0;
}

/* 
 * hello_exit() - called when module is removed
 */
static void hello_exit(void)
{
	printk(KERN_INFO "Graceful Exit.\n");
}
 
/* register module start and end functions */
module_init(hello_init);
module_exit(hello_exit);
 
/* basic description of module */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zed");
MODULE_DESCRIPTION("Module looks at block device");