/*
MakeBlankSav (C) 2005,2017,2025 Tatsuhiko Shoji
The sources for SAVList are distributed under the MIT open source license
*/
#include	<stdio.h>
#include	<stdlib.h>

#define SECTORS 14
#define SECTORSIZE 512


/**
 * Show how to Use
 */
void usage(void)
{
	fprintf(stderr,"MakeBlankSav Version 1.0.0 By Tatsuhiko Syoji(Tatsu) 2025\n\n");
	fprintf(stderr,"Usage:MakeBlankSav msxPlayer_sav_file\n");
	exit(1);
}

/**
 * @brief Set boot sector infomation.
 *
 * @param sec 512 bytes buffer for boot sector information
 *
 * This function writes FAT12 format floppy disk information.
 */
void makeBootSector(unsigned char *sec)
{
	/* Sector size */
	sec[0x0b] = 0;
	sec[0x0c] = 2;
	/* Cluster size */
	sec[0x0d] = 2;
	/* First FAT secxtor no. */
	sec[0x0e] = 1;
	/* Copies of FAT */
	sec[0x10] = 2;
	/* Root directory entries */
	sec[0x11] = 112;
	/* Total sectors */
	sec[0x13] = 0xa0;
	sec[0x14] = 0x05;
	 /* Media descriptor */
	sec[0x15] = 0xf9;
	/* Sectors per FAT */
	sec[0x16] = 3;
	/* Sectors per track */
	sec[0x18] = 9;
	/* Disk surcaces */
	sec[0x1a] = 2;

	/* Boot signature */
	sec[0x1fe] = 0x55; 
	sec[0x1ff] = 0xaa;
}

/**
 * Create blank MSX PLAYer SAV
 *
 * @param argc Argument count
 * @param argv Argument array
 * @return execution result 0:success 1:error
 */
int makeBlankSav(char *savFile)
{
	FILE *fpo;
	unsigned char sec[SECTORSIZE];
	unsigned char head[4];
	long i;
	int result = 0,ioCount;

	fpo = fopen(savFile,"wb");
	if (fpo == NULL){
		return 1;
	}

	for (i = 0;i < SECTORS;i++){

		head[0] = (unsigned char)(i & 0xff);
		head[1] = (unsigned char)((i >> 8) & 0xff);
		head[2] = (unsigned char)((i >> 16) & 0xff);
		head[3] = (unsigned char)((i >> 24) & 0xff);

		/* Write sector no. */
		ioCount = fwrite(head, 4, 1,fpo);
		if (ioCount < 1){
			fprintf(stderr,"Sector No Write error.\n");
			result = 1;
			break;
		}

		/* Write sector contents */
		memset(sec, 0, SECTORSIZE);

		/* MSX PLAYer don't recognize boot sector */
		/* So, we only need to create FAT ID.  */
		switch (i) {
			case 0:
				/* Boot sector */
				makeBootSector(sec);
				break;
			case 1:
			case 4:
				/* FAT12 */
				/* FAT(Primary and copy) */
				/* FAT12 FAT ID */
				sec[0] = 0xf8;
				/* Dummy */
				sec[1] = 0xff;
				/* Dummy */
				sec[2] = 0xff;
				break;
			default:
				break;
		}
		
		/* Write sector contents */
		fwrite(sec, SECTORSIZE, 1, fpo);
		if (ioCount < 1){
			fprintf(stderr,"Write error.\n");
			result = 1;
			break;
		}
	}
	
	fclose(fpo);
	
	return result;
}

/**
 * Entry point
 *
 * @param argc Argument count
 * @param argv Argument array
 * @return execution result 0:success 1:error
 */
int main(int argc,char *argv[])
{
	int result;
	
	if (argc < 2){
		usage();
	}
	result = makeBlankSav(argv[1]);
	
	return result;
}
