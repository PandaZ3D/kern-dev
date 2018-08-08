/* 
 * File: ffblock.c
 * Author: Allen Aboytes
 * Date: 7/12/18
 *
 * Desc: program written in user space finds
 * 		 free space block in a block device.
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

#define USAGE			("usage: %s -p <device-path>\n")

/* structures or unions */
static union raw_data 
{
	uint8_t b;
	uint16_t sb;
	uint32_t lb;
	uint8_t buf[32];
} buffer;


static char* dev_path;

/* function to read raw data from block device */
ssize_t data_read(int bd, union raw_data* rdb, size_t bytes, off_t whence);
/* function to dump raw data like hexdump */
void dump(uint8_t* buf, int len);

int main(int argc, char** argv)
{	
	if(argc < 2)
	{
		fprintf(stderr, USAGE, argv[0]);
		exit(1);
	}
	
	/* get command line options */
	int opt;
	while((opt = getopt(argc, argv, "p:")) != -1)
	{
		switch(opt)
		{
			case 'p': /* path to block device */
			{	dev_path = strdup(optarg);
				break;
			}
			case '?':
			default:
				fprintf(stderr, USAGE, argv[0]);
				exit(1);
		}
	}

	/* open device */
	int devfd = open(dev_path, O_RDONLY);
		err(devfd);
	
	/* read first logical sector */
	uint8_t sector[SECTOR_SIZE];

	int bytes = read(devfd, sector, SECTOR_SIZE);
		err(bytes);

	/* boot sector of fat32  partition */
	struct fat_boot_sector* fat_sb;
	fat_sb = (struct fat_boot_sector*) sector;

	/* calculate total number of clusters (blocks) in fat */
	uint8_t sector_per_cluster = fat_sb->sec_per_clus;
	uint32_t disk_clusters = fat_sb->total_sect / sector_per_cluster;
	uint32_t last_cluster = disk_clusters - (fat_sb->reserved 
		+ fat_sb->fat32.length * fat_sb->fats) / sector_per_cluster + 1;

	/* fat byte offset */
	off_t fat_off =  SECTOR_SIZE * fat_sb->reserved;

	/* descriptor to fat */
	int stat = lseek(devfd, fat_off, SEEK_SET);
		err(stat);	


	/* pointer to fat */
	uint32_t *FAT;
	
	/* cluster, first free, last free index */
	uint32_t curr_clus = 0, first_free, last_free;
	/* current and previous entry values and index */
	uint32_t c, p = FAT_ENT_EOF, entry;

	/* scan fat for free blocks marking start and end */
	do
	{
		/* read a sector of data */
		bytes = read(devfd, sector, SECTOR_SIZE);
			err(bytes);
		
		/* set buffer index */
		entry = 0;
		FAT = (uint32_t*) sector;
		do
		{
			/* get entry from fat */
			c = FAT[entry];
		
			/* check for in condition case */
			if(p == FAT_ENT_EOF && c == FAT_ENT_FREE)
				first_free = curr_clus;

			/* check for out condition case */
			if(p == FAT_ENT_FREE && c == FAT_ENT_EOF)
			{
				last_free = curr_clus - 1;
			
				/* now print since both are set */
				if(first_free == last_free)
					printf("%d\n", first_free);
				else
					printf("%d %d\n", first_free, last_free);
			}

			/* assign previous value */
			p = c;
		} while(++entry < SECTOR_SIZE/4 && ++curr_clus < last_cluster);
	} while(++curr_clus < last_cluster);


	/* edge case for last index being free */
	if(first_free > last_free)
	{
		/* now print since both are set */
		if(first_free == last_cluster - 1)
			printf("%d\n", first_free);
		else
			printf("%d %d\n", first_free, last_cluster - 1);
	}

	/* close device */
	stat = close(devfd);
		err(stat);

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