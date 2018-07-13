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
#include <getopt.h> /* optarg */

/* user debugging and I/O */
#include <stdio.h>

/* macros and macro functions */
#define err(stat)\
if(stat == -1) exit(1);

int main(int argc, char** argv)
{
	char* dev_path = "/dev/sdb1";

	int devfd = open(dev_path, O_RDONLY);
		err(devfd);

	return 0;
}