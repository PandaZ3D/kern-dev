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
#include <stdio.h>	/* perror() printf() */
#include <stdlib.h> /* exit() */
#include <string.h>
#include <ctype.h>	/* isprint() */
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

struct fat_fs
{
	uint8_t	sector_cl;	 	/* sectors per cluster */
	uint16_t sector_sz;	 	/* sector size */
	uint32_t root_cl;		/* start of root cluster */
	int fsinfo_d;			/* descriptor pointing to fsinfo boot */
	int fat_d;				/* descriptor pointing to fat */
	int data_d;				/* descriptor pointing to data */
};

void printfs(struct fat_fs*);

/* function to read raw data from block device */
ssize_t data_read(int bd, union raw_data* rdb, size_t bytes, off_t whence);

void dump(uint8_t* buf, int len);

int main(int argc, char** argv)
{
	char* dev_path = "/dev/sdb1";
	
	/* open device */
	printf("Opening %s ...\n", dev_path);
	int devfd = open(dev_path, O_RDONLY);
		err(devfd);

	printf("\n");

	/* boot sector of fat32  partition */
	struct fat_boot_sector* fat_sb;

	/* fat fs object */
	struct fat_fs fat32;

	/* now read boot sector */
	int stat = lseek(devfd, 0, SEEK_SET);
		err(stat);

	uint8_t sector[SECTOR_SIZE];

	int bytes = read(devfd, sector, SECTOR_SIZE);
		err(bytes);

	fat_sb = (struct fat_boot_sector*) sector;

	/* initialize fat_fs object from boot */
	fat32.sector_sz = READ_SHORT(fat_sb->sector_size);
	fat32.sector_cl = fat_sb->sec_per_clus;
	fat32.root_cl = fat_sb->fat32.root_cluster;

	/* descriptors to device partitions */
	fat32.fsinfo_d = dup(devfd);
	fat32.fat_d = dup(devfd);
	fat32.data_d = dup(devfd);

	/* fsinfo offset */
	off_t part_off = fat_sb->fat32.info_sector * fat32.sector_sz;

	stat = lseek(fat32.fsinfo_d, part_off, SEEK_SET);
		err(stat);

	/* fat offset */
	part_off = fat32.sector_sz * fat_sb->reserved;

	stat = lseek(fat32.fat_d, part_off, SEEK_SET);
		err(stat);	

	uint32_t i, p = 0, f = 0;
	uint32_t total_ent = (fat_sb->fat32.length * fat32.sector_sz) / 4;
	printf("total_ent: %d\n", total_ent);

	for(i = FAT_START_ENT; i<total_ent; i++)
	{
		bytes = read(fat32.fat_d, &buffer, 4);
			err(bytes);
		if(buffer.lb == FAT_ENT_FREE)
		{
			f++;
		}
		if((p % 64) == 63)
			printf("\n");
	}

	printf("total free: %d\n", f);

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

void printfs(struct fat_fs* fs)
{
	printf("Sector Size: %d\n", fs->sector_sz);
	printf("Sectors per Cluster: %d\n", fs->sector_cl);
	printf("Root Cluster: %d\n", fs->root_cl);	
}

/* dumps raw memory similar to hexdump */
void dump(uint8_t* buf, int len)
{
	uint8_t b;
	int i, j;

	for(i = 0; i < len; i++)
	{
		b = buf[i];
		printf("%02x ", b);

		if(((i % 16) == 15) | (i == (len - 1)))
		{
			printf("|");
			for(j = i - 15; j < i; j++)
			{
				printf("%c", (isprint(buf[j]))? buf[j] : '.');
			}
			printf("|\n");
		}
	}
}