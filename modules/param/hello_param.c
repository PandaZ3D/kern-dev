/* 
 * Allen Aboytes 
 * 8/1/2018
 *
 * hello_param.c - prints parameters passed on build
 *
 * Module taints kernel on build
 */
 
 /* standard headers for kernel modules */
 #include <linux/init.h>
 #include <linux/module.h>
 #include <linux/kernel.h>
 
 #define PERM	0000 /* parameter permissions */

 /* message passed in from user */
 static char* msg;

/* set string to msg pointer */
module_param(msg, charp, PERM);
MODULE_PARM_DESC(msg, "A string from user");

 /* 
  * hello_init() - called when module is loaded
  * RETURN: 0 if successfully loaded, nonzero else
  */
static int hello_init(void)
{
	printk(KERN_INFO "Here is the secret salsa recepie: %s.\n", msg);
	return 0;
}

/* 
 * hello_exit() - called when module is removed
 */
 static void hello_exit(void)
 {
	 printk(KERN_INFO "Now don't tell anyone.\n");
 }
 
 /* register module start and end functions */
 module_init(hello_init);
 module_exit(hello_exit);
 
 /* basic description of module */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zed");
MODULE_DESCRIPTION("A hello world module");