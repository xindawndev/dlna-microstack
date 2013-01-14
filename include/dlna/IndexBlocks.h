#ifndef _INDEXBLOCKS_H_
#define _INDEXBLOCKS_H_


typedef void *IndexBlocks;


IndexBlocks IndexBlocks_Create();
void IndexBlocks_Destroy(IndexBlocks blocks);

int IndexBlocks_AddBlock(IndexBlocks blocks, int streamOffset, int length, int trackCount);

int IndexBlocks_GetTrackCount(IndexBlocks blocks);
int IndexBlocks_GetTrackRangeInfo(IndexBlocks blocks, int trackNumber, int* byteOffset, int* length, int* trackOffset);

#endif /* __INDEXBLOCKS_H__ */
