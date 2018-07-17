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

/* macros and macro functions */
#define err(stat)\
if(stat == -1) {perror("filesystem:"); exit(1);}

/* structures or unions */
static union raw_data 
{
	uint8_t b;
	uint16_t sb;
	uint32_t lb;
	uint8_t buf[32];
} buffer;

struct fat32_sb
{
	uint8_t nfat;		 	/* number of fat (1 or 2) */
	uint8_t	sector_cl;	 	/* sectors per cluster */
	uint8_t fsinfo_sec;	 	/* sector number of fsinfo */
	uint16_t sector_sz;	 	/* sector size */
	uint16_t sector_res; 	/* sectors reserved */
	uint16_t sector_fat;	/* sectors per fat */
	uint32_t nsectors;		/* sectors on disk */
	uint32_t root_cl;		/* start of root cluster */
	uint8_t volume_id[8];	/* name of fat sys */
};

static struct fat32_fs {
	struct fat32_sb sb32;
	int fatd, datad;
} fatfs;

/* function to read raw data from block device */
ssize_t data_read(int bd, union raw_data* rdb, size_t bytes, off_t whence);
void get_super_block(int bd, struct fat32_sb* sb);
void get_fat32_fs(int bd, struct fat32_fs* fs);
void print_vfs(struct statvfs vfs);
void print_fs(struct statfs fs);

int main(int argc, char** argv)
{
	char* dev_path = "/dev/sdb1";
	char* mount_path = "/home/zed/Desktop/vfat";

	printf("Opening %s ...\n", mount_path);
	int devfd = open(mount_path, O_RDONLY);
		err(devfd);

	/* vfs attr */
	struct statvfs vfs;
	int stat = fstatvfs(devfd, &vfs);
		err(stat);

	//print_vfs(vfs);
	//printf("\n");

	/* fs attr */
	struct statfs fs;
	stat = fstatfs(devfd, &fs);
		err(stat);
	//print_fs(fs);
	//printf("\n");

	printf("Closing %s ...\n", mount_path);
	stat = close(devfd);
		err(stat);

	printf("Opening %s ...\n", dev_path);
	devfd = open(dev_path, O_RDONLY);
		err(devfd);

	printf("\n");


	/* sector size */
	data_read(devfd, &buffer, 2, 0x0b);
	printf("Sector Size: %d\n", buffer.sb);

	/* sectors per cluster (block) */
	data_read(devfd, &buffer, 1, 0x0d);
	printf("Sectors per Cluster: %d\n", buffer.b);

	/* reserved sectors */
	data_read(devfd, &buffer, 2, 0x0e);
	printf("Reserved Sectors: %d\n", buffer.sb);

	/* number of FAT */
	data_read(devfd, &buffer, 1, 0x10);
	printf("Number of FAT: %d\n", buffer.b);

	/* sectors per FAT */
	data_read(devfd, &buffer, 2, 0x24);
	printf("Sectors per FAT: %d\n", buffer.sb);

	/* total sectors per disk */
	data_read(devfd, &buffer, 4, 0x20);
	printf("Sectors per Disk: %d\n", buffer.lb);

	/* rood directory first cluster */
	data_read(devfd, &buffer, 4, 0x2C);
	printf("Root Dir first Cluster: %d\n", buffer.lb);

	/* fsinfo sector offset */
	data_read(devfd, &buffer, 2, 0x030);
	printf("FS Info Sector: %d\n", buffer.sb);

	/* moving onto FS info sector */
	uint16_t fsi_offs = buffer.sb * 512;

	/* volume ID */
	stat = data_read(devfd, &buffer, 8, 0x52);
	buffer.buf[stat] = 0;
	printf("Volume ID: %s\n", buffer.buf);

	printf("\n");

	/* last free cluster */
	data_read(devfd, &buffer, 4, fsi_offs + 0x1E8);
	printf("Number of free Clusters: %d\n", buffer.lb);	

	/* free cluster location */
	data_read(devfd, &buffer, 4, fsi_offs + 0x1EC);
	printf("Last Allocated Cluster: %d\n", buffer.lb);	

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

void get_super_block(int bd, struct fat32_sb* sb)
{
	
}

void get_fat32_fs(int bd, struct fat32_fs* fs)
{

}

void print_vfs(struct statvfs vfs)
{
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
}
void print_fs(struct statfs fs)
{
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
}
