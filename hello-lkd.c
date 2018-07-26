/* 
 * Allen Aboytes 
 * 7/25/2018
 *
 * hello-lkd.c - Hello World Kernel Module
 * Linux Kernel Development - Robert Love
 */
 
 /* standard headers for kernel modules */
 #include <linux/init.h>
 #include <linux/module.h>
 #include <linux/kernel.h>
 
 /* 
  * hello_init() - called when module is loaded
  * RETURN: 0 if successfully loaded, nonzero else
  */
static int hello_init(void)
{
	printk(KERN_ALERT "I'm a man on the inside.\n");
	return 0;
}

/* 
 * hello_exit() - called when module is removed
 */
 static void hello_exit(void)
 {
	 printk(KERN_ALERT "Now time for my escape.\n");
 }
 
 /* register module start and end functions */
 module_init(hello_init);
 module_exit(hello_exit);
 
 /* basic description of module */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zed");
MODULE_DESCRIPTION("A hello world module");