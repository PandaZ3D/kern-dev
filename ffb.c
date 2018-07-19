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
		-m mount path
	open()
	stat() --> for a block device
		look specifically at vfat

*/

/* main include directives */
#include <fcntl.h> /* open() */
#include <unistd.h> /* close() read() getopt() */
#include <sys/statvfs.h> /* fstatvfs() */
#include <sys/vfs.h> /* fstatfs() */
#include <stdint.h> /* uint#_t */
#include <getopt.h> /* optarg */

/* user debugging and I/O */
#include <stdio.h>
#include <stdlib.h> /* exit() */
#include <string.h>
#include <errno.h>

/* kernel includes */
#include <linux/msdos_fs.h>

/* macros and macro functions */
#define err(stat)\
if(stat == -1) {perror("filesystem:"); exit(1);}

#define READ_SHORT(s)	(s[1] << 8 | s[0])
#define READ_LONG(l)	((l[3] << 32 | l[2] << 16 | (READ_SHORT(l)))
/* structures or unions */
static union raw_data 
{
	uint8_t b;
	uint16_t sb;
	uint32_t lb;
	uint8_t buf[32];
} buffer;

/* function to read raw data from block device */
ssize_t data_read(int bd, union raw_data* rdb, size_t bytes, off_t whence);

int main(int argc, char** argv)
{
	char* dev_path = "/dev/sdb1";
	
	printf("Opening %s ...\n", dev_path);
	int devfd = open(dev_path, O_RDONLY);
		err(devfd);

	printf("\n");

	/* boot sector of fat32  partition */
	struct fat_boot_sector* fat_sb;

	data_read(devfd, &buffer, 2, 0x0b);
	uint16_t sector_size = buffer.lb;
	printf("sector size: %d\n", sector_size);

	/* now read whole block */
	int stat = lseek(devfd, 0, SEEK_SET);
		err(stat);

	uint8_t sector[SECTOR_SIZE];

	int bytes = read(devfd, sector, SECTOR_SIZE);
		err(bytes);

	fat_sb = (struct fat_boot_sector*) sector;

//	memcpy(&buffer, fat_sb.sector_size, 2);

	printf("read %d bytes\n", bytes);
	printf("fat boot sector size: %d\n\n", 
		READ_SHORT(fat_sb->sector_size));
	
	printf("Closing %s ...\n", dev_path);
	stat = close(devfd);
		err(stat);

	printf("(END)\n");

	return 0;
}

ssize_t data_read(int bd, union raw_data* rdb, size_t bytes, off_t whence)
{
	int stat = lseek(bd, whence, SEEK_SET);
		err(stat);

	int bytes_read = read(bd, rdb, bytes);
		err(bytes_read);

	return bytes_read;
}