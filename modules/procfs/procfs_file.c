/* 
 * Allen Aboytes 
 * 7/26/2018
 *
 * procfs_file.c - creates a file in /proc
 *
 */
 
/* standard headers for kernel modules */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h> /* user data transfer */
#include <linux/cred.h>

/* struct proc definition */
#include <internal.h>


#define NAME 	"newfile"	/* file name */
#define PERM 	0444 		/* file permissions */
#define PARENT 	NULL

/* procfs file to be implemented */
static struct proc_dir_entry* new_proc_file;
/*
 * called whenever file is open, increment reference count
 */
int proc_file_open(struct inode* inode, struct file* file)
{
	try_module_get(THIS_MODULE);
	return 0;
}

/*
 * similarly to open we care only about the reference count
 */
int proc_file_close(struct inode* inode, struct file* file)
{
	module_put(THIS_MODULE);
	return 0;
}

/* 
 * called whenver the procfile is to be read
 *
 * kernbuf - buffer allocated by kernel
 * offset- offset within a file
 * 
 */
static ssize_t 
proc_file_read(struct file* file, char* kernbuf, size_t len, loff_t* offset)
{
	static char eof = 0;
	int bytes_in_buf = 0;

	/* mechanism for issueing eof */
	if(eof)
	{
		eof = 0;
		return 0;
	}

	bytes_in_buf = sprintf(kernbuf, "Hello from within /proc/%s\n", NAME);
	eof = 1;

	return bytes_in_buf;
}

static ssize_t 
proc_file_write(struct file* file, const char* kernbuf, size_t len, loff_t* offset)
{
	return 0;
}

/* 
 * permissions based on what operation is allowed
 * 
 * 0 − Execute
 * 2 − Write
 * 4 − Read
 */
static int 
proc_file_perm(struct inode *inode, int op)
{
	/* make file read only */
	if((op & MAY_READ) /*&& (__kuid_val(current_cred()->uid) == 0)*/)
		return 0;

	return -EACCES;
}

/* file operations for proc file */
static struct file_operations pfops = {
	.read = proc_file_read,
	.write = proc_file_write,
	.open = proc_file_open,
	.release = proc_file_close,
};

/* file permissions */
static struct inode_operations piops = {
	.permission = proc_file_perm,
};

/* 
 * creates proc file entry in inode table and
 * registers file attributes and reading function.
 *
 * RETURN: 0 if successfully loaded, nonzero else
 */
static int proc_file_init(void)
{
	/* register proc file */
	new_proc_file = proc_create(NAME, PERM, NULL, &pfops);

	if(new_proc_file)
	{
		/* set proc file attributes */
		new_proc_file->proc_fops = &pfops;
		new_proc_file->proc_iops = &piops;
		new_proc_file->mode = S_IFREG | S_IRUGO | S_IWUSR;
		new_proc_file->uid = KUIDT_INIT(0);
		new_proc_file->gid = KGIDT_INIT(0);
		new_proc_file->size = 0;
		
		return 0;
	}

	/* null reference, must remove proc entry */
	remove_proc_entry(NAME, NULL);

	printk(KERN_ALERT "Error: could not initialize new proc file");

	return -ENOMEM;
}

/* 
 * 
 */
static void proc_file_exit(void)
{
 	remove_proc_entry(NAME, NULL);
}
 
/* register module start and end functions */
module_init(proc_file_init);
module_exit(proc_file_exit);
 
/* basic description of module */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zed");
MODULE_DESCRIPTION("Simple Module to create file in /proc");
