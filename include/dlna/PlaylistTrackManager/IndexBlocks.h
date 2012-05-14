#ifndef __INDEXBLOCKS_H__
#define __INDEXBLOCKS_H__


typedef void *IndexBlocks;


IndexBlocks IndexBlocks_Create();
void IndexBlocks_Destroy(IndexBlocks blocks);

int IndexBlocks_AddBlock(IndexBlocks blocks, int streamOffset, int length, int trackCount);

int IndexBlocks_GetTrackCount(IndexBlocks blocks);
int IndexBlocks_GetTrackRangeInfo(IndexBlocks blocks, int trackNumber, int* byteOffset, int* length, int* trackOffset);

#endif /* __INDEXBLOCKS_H__ */
