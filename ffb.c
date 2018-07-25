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

	/* read first logical sector */
	uint8_t sector[SECTOR_SIZE];

	int bytes = read(devfd, sector, SECTOR_SIZE);
		err(bytes);

	/* boot sector of fat32  partition */
	struct fat_boot_sector* fat_sb;
	fat_sb = (struct fat_boot_sector*) sector;

	/* descriptor to fat */
	int fat_d = dup(devfd);
	
	/* calculate total number of clusters (blocks) in fat */
	uint8_t sector_per_cluster = fat_sb->sec_per_clus;
	uint16_t sector_size = READ_SHORT(fat_sb->sector_size);
	uint32_t total_clusters = fat_sb->total_sect / sector_per_cluster;
	uint32_t valid_clusters = total_clusters - (fat_sb->reserved 
		+ fat_sb->fat32.length * fat_sb->fats) / sector_per_cluster + 1;

	/* fat byte offset */
	off_t fat_off =  sector_size * fat_sb->reserved;

	int stat = lseek(fat_d, fat_off, SEEK_SET);
		err(stat);	


/////////////////////////////////////////////////////////////

	/* current, first free, and last free index */
	/* current and previous entry values */
	uint32_t curr_clus = FAT_ENT_FREE, *FAT;
	uint32_t c, p = FAT_ENT_EOF, f, l, bi;

	/* scan fat for free blocks marking start and end */
	do
	{
		/* read a sector of data */
		bytes = read(fat_d, sector, sector_size);
			err(bytes);
		
		/* set buffer index */
		bi = 0;
		FAT = (uint32_t*) sector;
		do
		{
			/* get entry from fat */
			c = FAT[bi];
		
			/* check for in condition case */
			if(p == FAT_ENT_EOF && c == FAT_ENT_FREE)
				f = curr_clus;

			/* check for out condition case */
			if(p == FAT_ENT_FREE && c == FAT_ENT_EOF)
			{
				l = curr_clus - 1;
			
				/* now print since both are set */
				if(f == l)
					printf("%d\n", f);
				else
					printf("%d %#d\n", f, l);
			}

			/* assign previous value */
			p = c;
		} while(bi++ < sector_size && curr_clus++ < valid_clusters);
	} while(curr_clus++ < valid_clusters);


	/* edge case for last index being free */
	if(f > l)
	{
		/* now print since both are set */
		if(f == valid_clusters - 1)
			printf("%#d\n", f);
		else
			printf("%#d %#d\n", f, valid_clusters - 1);
	}

/////////////////////////////////////////////////////////////

	printf("\n");
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