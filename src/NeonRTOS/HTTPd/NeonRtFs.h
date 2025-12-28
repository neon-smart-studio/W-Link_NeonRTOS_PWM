
#ifndef NEONRTFS_H
#define NEONRTFS_H

#include "NeonRtFsFormat.h"

#ifdef	__cplusplus
extern "C" {
#endif

#define NEONRTFS_DEBUG  0

//#define NEONRTFS_HEATSHRINK

#define FLASH_SECTOR_SIZE 512

// to be able to use Heatshrink-compressed NeonRtFsfs images.
//#define NEONRTFS_HEATSHRINK

typedef enum {
	NEONRTFS_INIT_RESULT_OK,
	NEONRTFS_INIT_RESULT_NO_IMAGE,
	NEONRTFS_INIT_RESULT_BAD_ALIGN,
	NEONRTFS_INIT_RESULT_ERR_MEM,
} NeonRtFsInitResult;

typedef struct NeonRtFsFile {
	NeonRtFsHeader header;
	char decompressor;
	int32_t posDecomp;
	uint32_t posStart;
	uint32_t posComp;
	char* decompData;
} NeonRtFsFile;

NeonRtFsInitResult NeonRtFsInit();
NeonRtFsFile *NeonRtFsOpen(char *fileName);
int NeonRtFsFlags(NeonRtFsFile *fh);
int NeonRtFsRead(NeonRtFsFile *fh, char *buff, int len);
void NeonRtFsClose(NeonRtFsFile *fh);

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif //NEONRTFS_H