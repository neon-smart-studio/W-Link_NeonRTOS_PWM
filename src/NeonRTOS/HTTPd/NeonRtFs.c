
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "NeonRTOS.h"

#include "NeonRtFs.h"

#ifdef NEONRTFS_HEATSHRINK
#include "heatshrink_config_custom.h"
#include "heatshrink_decoder.h"
#endif

#define min(a,b) ((a)<(b)?(a):(b))  /**< Find the minimum of 2 numbers. */

bool NeonRtFsInitDone = false;
uint32_t NeonRtFsData_StartAddr = 0;

NeonRTOS_LockObj_t Flash_Access_MutexHandle = NULL;

extern unsigned char neonrtfs_img[];
extern unsigned int neonrtfs_img_len;
/*
Available locations, at least in my flash, with boundaries partially guessed. This
is using 0.9.1/0.9.2 SDK on a not-too-new module.
0x00000 (0x10000): Code/data (RAM data?)
0x10000 (0x02000): Gets erased by something?
0x12000 (0x2E000): Free (filled with zeroes) (parts used by ESPCloud and maybe SSL)
0x40000 (0x20000): Code/data (ROM data?)
0x60000 (0x1C000): Free
0x7c000 (0x04000): Param store
0x80000 - end of flash

Accessing the flash through the mem emulation at 0x40200000 is a bit hairy: All accesses
*must* be aligned 32-bit accesses. Reading a short, byte or unaligned word will result in
a memory exception, crashing the program.
*/
NeonRtFsInitResult NeonRtFsInit() {
	/*
	if (((int)neonrtfs_img & 3) != 0) {
		return NEONRTFS_INIT_RESULT_BAD_ALIGN;
	}
*/
	// check if there is valid header at address
	NeonRtFsHeader testHeader;
	memcpy(&testHeader, neonrtfs_img, sizeof(NeonRtFsHeader));
        
	if (testHeader.magic != NEONRTFS_MAGIC) {
		return NEONRTFS_INIT_RESULT_NO_IMAGE;
	}
        
	NeonRtFsInitDone = true;
    
	return NEONRTFS_INIT_RESULT_OK;
}

//Copies len bytes over from dst to src, but does it using *only*
//aligned 32-bit reads. Yes, it's no too optimized but it's short and sweet and it works.

//ToDo: perhaps memcpy also does unaligned accesses?
void readFlashUnaligned(char *dst, uint8_t* src, int len) {
	if(dst==NULL){
#if NEONRTFS_DEBUG==1
		UART_Printf("[NeonRTFS] Invalid Read Buf Pointer!\n");
#endif
		return;
	}
	if(len==0){
		return;
	}
  
	memcpy(dst, src, len);
}

// Returns flags of opened file.
int NeonRtFsFlags(NeonRtFsFile *fh) {
	if (fh == NULL) {
#if NEONRTFS_DEBUG==1
		UART_Printf("[NeonRTFS] File handle not ready\n");
#endif
		return -1;
	}

	return (int)fh->header.flags;
}

//Open a file and return a pointer to the file desc struct.
NeonRtFsFile *NeonRtFsOpen(char *fileName) {
	if (NeonRtFsInitDone == false) {
#if NEONRTFS_DEBUG==1
		UART_Printf("[NeonRTFS] Call NeonRtFsInit first!\n");
#endif
		return NULL;
	}
	uint8_t* s = neonrtfs_img;
	uint8_t* p = s;
	char namebuf[256+1];
	NeonRtFsHeader h;
	NeonRtFsFile *r;
	//Strip initial slashes
	while(fileName[0]=='/') fileName++;
	//Go find that file!
	while(1) {
		//Grab the next file header.
		memcpy(&h, p, sizeof(NeonRtFsHeader));
		//Grab the name of the file.
		p+=sizeof(NeonRtFsHeader); 

		if (h.magic!=NEONRTFS_MAGIC) {
#if NEONRTFS_DEBUG==1
			UART_Printf("[NeonRTFS] Magic mismatch. NeonRTFS image broken.\n");
#endif
                        NeonRtFsInitDone = false;
                        
			return NULL;
		}
		if (h.flags&FLAG_LASTFILE) {
#if NEONRTFS_DEBUG==1
			UART_Printf("[NeonRTFS] End of image.\n");
#endif
			return NULL;
		}

		memcpy(namebuf, p, min(256, h.nameLen));
		namebuf[min(256, h.nameLen)] = '\0'; // 確保以空字元終止

//		info("Found file '%s'. Namelen=%x fileLenComp=%x, compr=%d flags=%d",
//				namebuf, (unsigned int)h.nameLen, (unsigned int)h.fileLenComp, h.compression, h.flags);
#if NEONRTFS_DEBUG==1
		UART_Printf("[NeonRTFS] Found File %s.\n", namebuf);
		UART_Printf("[NeonRTFS] File Length %d Bytes\n", h.fileLenComp);
		UART_Printf("[NeonRTFS] File Content Start at %x\n", p+h.nameLen);
#endif
		if (strncmp(namebuf, fileName, min(strlen(namebuf), strlen(fileName)))==0)
		{
			//Yay, this is the file we need!
			p += h.nameLen; //Skip to content.
			r = (NeonRtFsFile *)mem_Malloc(sizeof(NeonRtFsFile)); //Alloc file desc mem
//			UART_Printf("Alloc %p", r);
			if (r==NULL) return NULL;

            memcpy(&r->header, &h, sizeof(NeonRtFsHeader));
			r->decompressor = h.compression;
			r->posComp = p;
			r->posStart = p;
			r->posDecomp = 0;
			if (h.compression==COMPRESS_NONE) {
				r->decompData=NULL;
#ifdef NEONRTFS_HEATSHRINK
			} else if (h.compression==COMPRESS_HEATSHRINK) {
				//File is compressed with Heatshrink.
				char parm;
				heatshrink_decoder *dec;
				//Decoder params are stored in 1st byte.
				readFlashUnaligned(&parm, r->posComp, 1);
				r->posComp++;
#if NEONRTFS_DEBUG==1
				UART_Printf("[NeonRTFS] Heatshrink compressed file; decode parms = %x\n", parm);
#endif
				dec=heatshrink_decoder_alloc(16, (parm>>4)&0xf, parm&0xf);
				r->decompData=(char*)dec;
#endif
			} else {
#if NEONRTFS_DEBUG==1
				UART_Printf("[NeonRTFS] Invalid compression: %d\n", h.compression);
#endif
				mem_Free(r);
				return NULL;
			}
#if NEONRTFS_DEBUG==1
			UART_Printf("[NeonRTFS] Open File %s\n", fileName);
#endif
			return r;
		}
		//We don't need this file. Skip name and file
		p += h.nameLen + h.fileLenComp;
		if ((int)(p-s)&3)
		{
			p+=4-((int)(p-s)&3); //align to next 32bit val
		}
	}
}

//Read len bytes from the given file into buff. Returns the actual amount of bytes read.
int NeonRtFsRead(NeonRtFsFile *fh, char *buff, int len) {
	int flen;
#ifdef NEONRTFS_HEATSHRINK
	int fdlen;
#endif
	if (fh==NULL) return 0;
#if NEONRTFS_DEBUG==1
        UART_Printf("[NeonRTFS] Read File\n");
#endif
		
        flen = fh->header.fileLenComp;
	//Cache file length.
	//Do stuff depending on the way the file is compressed.
	if (fh->decompressor==COMPRESS_NONE) {
		int toRead;
		toRead=flen-(fh->posComp-fh->posStart);
		if (len>toRead) len=toRead;
#if NEONRTFS_DEBUG==1
  		UART_Printf("[NeonRTFS] Reading %d Bytes From %x\n", len, (unsigned int)fh->posComp);
#endif
		readFlashUnaligned(buff, fh->posComp, len);
#if NEONRTFS_DEBUG==1
                UART_Printf("[NeonRTFS] Read Image Data %s\n", buff);
#endif
		fh->posDecomp+=len;
		fh->posComp+=len;
#if NEONRTFS_DEBUG==1
                UART_Printf("[NeonRTFS] Done Reading %d Bytes, Pos=%x\n", len, fh->posComp);
#endif
		return len;
#ifdef NEONRTFS_HEATSHRINK
	} else if (fh->decompressor==COMPRESS_HEATSHRINK) {
                fdlen = fh->header.fileLenDecomp;
		int decoded=0;
		size_t elen, rlen;
		char ebuff[16];
		heatshrink_decoder *dec=(heatshrink_decoder *)fh->decompData;
//		UART_Printf("Alloc %p", dec);
		if (fh->posDecomp == fdlen) {
			return 0;
		}

		// We must ensure that whole file is decompressed and written to output buffer.
		// This means even when there is no input data (elen==0) try to poll decoder until
		// posDecomp equals decompressed file length

		while(decoded<len) {
			//Feed data into the decompressor
			//ToDo: Check ret val of heatshrink fns for errors
			elen=flen-(fh->posComp - fh->posStart);
			if (elen>0) {
				readFlashUnaligned(ebuff, fh->posComp, 16);
				heatshrink_decoder_sink(dec, (uint8_t *)ebuff, (elen>16)?16:elen, &rlen);
				fh->posComp+=rlen;
			}
			//Grab decompressed data and put into buff
			heatshrink_decoder_poll(dec, (uint8_t *)buff, len-decoded, &rlen);
			fh->posDecomp+=rlen;
			buff+=rlen;
			decoded+=rlen;

//			UART_Printf("Elen %d rlen %d d %d pd %ld fdl %d",elen,rlen,decoded, fh->posDecomp, fdlen);

			if (elen == 0) {
				if (fh->posDecomp == fdlen) {
//					info("Decoder finish");
					heatshrink_decoder_finish(dec);
				}
				return decoded;
			}
		}
		return len;
#endif
	}
	return 0;
}

//Close the file.
void NeonRtFsClose(NeonRtFsFile *fh) {
	if (fh==NULL) return;
#if NEONRTFS_DEBUG==1
        UART_Printf("[NeonRTFS] Close File\n");
#endif
#ifdef NEONRTFS_HEATSHRINK
	if (fh->decompressor==COMPRESS_HEATSHRINK) {
		heatshrink_decoder *dec=(heatshrink_decoder *)fh->decompData;
		heatshrink_decoder_free(dec);
//		UART_Printf("Freed %p", dec);
	}
#endif
//	UART_Printf("Freed %p", fh);
	mem_Free(fh);
}
