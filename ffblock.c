/* 
 * File: ffblock.c
 * Author: Allen Aboytes
 * Date: 7/12/18
 *
 * Desc: program written in user space finds
 * 		 free space block in a block device.
 */

/* brain storm main functions needed to perform task */
/*
	flags for modularity (getopt)
		-p path to device
		-f filesystem type
	open()
	stat() --> for a block device
		look specifically at vfat

*/

/* main include directives */
#include <fcntl.h> /* open() */
#include <unistd.h> /* close() read() getopt() */
#include <sys/statvfs.h> /* fstatvfs() */
#include <sys/vfs.h> /* fstatfs() */
#include <getopt.h> /* optarg */

/* user debugging and I/O */
#include <stdio.h>
#include <stdlib.h> /* exit() */
#include <string.h>
#include <errno.h>

/* macros and macro functions */
#define err(stat)\
if(stat == -1) {perror("filesystem:"); exit(1);}

int main(int argc, char** argv)
{
	char* dev_path = "/dev/sdb1";

	printf("Opening %s ...\n\n", dev_path);
	int devfd = open(dev_path, O_RDONLY);
		err(devfd);

	struct statvfs vfs;
	int stat = fstatvfs(devfd, &vfs);
		err(stat);

	printf("Filesystem Attributes (statvfs):\n");
	printf("\tf_bsize = %ld\n", vfs.f_bsize);
	printf("\tf_frsize = %ld\n", vfs.f_frsize);
	printf("\tf_blocks = %ld\n", vfs.f_blocks);
	printf("\tf_bfree = %ld\n", vfs.f_bfree);
	printf("\tf_bavail = %ld\n", vfs.f_bavail);
	printf("\tf_files = %ld\n", vfs.f_files);
	printf("\tf_ffree = %ld\n", vfs.f_ffree);
	printf("\tf_favail = %ld\n", vfs.f_favail);
	printf("\tf_fsid = %#x\n", vfs.f_fsid);
	printf("\tf_flag = %#x\n", vfs.f_flag);
	printf("\tf_namemax = %#x\n", vfs.f_namemax);
	
	printf("\n");

	struct statfs fs;
	stat = fstatfs(devfd, &fs);
		err(stat);

	printf("Filesystem Attributes (statfs):\n");
	printf("\tf_type = %#08x\n", fs.f_type);
	printf("\tf_bsize = %ld\n", fs.f_bsize);
	printf("\tf_frsize = %ld\n", fs.f_frsize);
	printf("\tf_blocks = %ld\n", fs.f_blocks);
	printf("\tf_bfree = %ld\n", fs.f_bfree);
	printf("\tf_bavail = %ld\n", fs.f_bavail);
	printf("\tf_files = %ld\n", fs.f_files);
	printf("\tf_ffree = %ld\n", fs.f_ffree);
	printf("\tf_fsid = %#x\n", fs.f_fsid);
	printf("\tf_flags = %#x\n", fs.f_flags);
	printf("\tf_namelength = %#x\n", fs.f_namelen);
	
	printf("\n");

	printf("Closing %s ...\n", dev_path);
	stat = close(devfd);
		err(stat);
	printf("(END)\n");

	return 0;
}