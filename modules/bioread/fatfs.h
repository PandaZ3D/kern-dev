#ifndef __FAT_FS_H__
#define __FAT_FS_H__

#include <fat.h>

/* 128kb is the whole sectors for FAT12 and FAT16 */
#define FAT_READA_SIZE		(128 * 1024)

/* struct definition */
struct fatent_operations {
	void (*ent_blocknr)(struct super_block *, int, int *, sector_t *);
	void (*ent_set_ptr)(struct fat_entry *, int);
	int (*ent_bread)(struct super_block *, struct fat_entry *,
			 int, sector_t);
	int (*ent_get)(struct fat_entry *);
	void (*ent_put)(struct fat_entry *, int);
	int (*ent_next)(struct fat_entry *);
};

void lock_fat(struct msdos_sb_info *sbi);

void unlock_fat(struct msdos_sb_info *sbi);

void fat_ent_reada(struct super_block *sb, struct fat_entry *fatent,
			  unsigned long reada_blocks);
int fat_ent_read_block(struct super_block *sb,
				     struct fat_entry *fatent);
int fat_ent_next(struct msdos_sb_info *sbi,
			       struct fat_entry *fatent);

#if 0
	sbi = MSDOS_SB(sb);
	fops = sbi->fatent_ops;

	/* lock reference to sb */
	lock_fat(sbi);
	if (sbi->free_clusters != -1 && sbi->free_clus_valid)
		goto out;
	
	reada_blocks = FAT_READA_SIZE >> sb->s_blocksize_bits; //number of blocks to read
	reada_mask = reada_blocks - 1; //reading mask
	cur_block = 0;

	fatent_init(&fatent);
	fatent_set_entry(&fatent, FAT_START_ENT);
	while (fatent.entry < sbi->max_cluster) {
		/* readahead of fat blocks */
		if ((cur_block & reada_mask) == 0) {
			unsigned long rest = sbi->fat_length - cur_block;
			fat_ent_reada(sb, &fatent, min(reada_blocks, rest));
		}
		cur_block++;

		err = fat_ent_read_block(sb, &fatent);
		if (err)
			goto out;

		do {
			c = fops->ent_get(&fatent);

			/* check for in condition case */
			if(p == FAT_ENT_EOF && c == FAT_ENT_FREE)
				first_free = curr_clus;

			/* check for out condition case */
			if(p == FAT_ENT_FREE && c == FAT_ENT_EOF)
			{
				last_free = curr_clus - 1;
			
				/* now print since both are set */
				if(first_free == last_free)
					printk(KERN_INFO "%ld\n", first_free);
				else
					printk(KERN_INFO "%ld %ld\n", first_free, last_free);
			}

			/* assign previous value */
			p = c;

			/* increment cluster count */
			curr_clus++;
		} while (fat_ent_next(sbi, &fatent));
	}
	
	/* edge case for last index being free */
	if(first_free > last_free)
	{
		/* now print since both are set */
		if(first_free == curr_clus - 1)
			printk(KERN_INFO "%ld\n", first_free);
		else
			printk(KERN_INFO "%ld %ld\n", first_free, curr_clus - 1);
	}

	fatent_brelse(&fatent);
	
	unlock_fat(sbi);
	
	#endif	
	
#endif /* __FAT_FS_H__ */