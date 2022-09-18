/*
 *  Woomïnstaller GX2 - Homebrew package installer for Wii U
 *
 *  Copyright (C) 2016          SALT
 *  Copyright (C) 2016          WulfyStylez
 *
 *  This code is licensed under the terms of the GNU LGPL, version 2.1
 *  see file LICENSE.md or https://www.gnu.org/licenses/lgpl-2.1.txt
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <coreinit/dynload.h>
#include <wut.h>
//#include "types.h"
#include "fsa.h"
#include "memory.h"

typedef struct
{
	void* ptr;
	uint32_t len;
	uint32_t unk;
}iovec_s;
/*
int (*IOS_Open)(char *path, int mode);
int (*IOS_Close)(int fd);
int32_t (*IOS_Ioctl)(int32_t fd, int32_t request, void *buffer_in, int32_t bytes_in, void *buffer_io, int32_t bytes_io);
int32_t (*IOS_Ioctlv)(int32_t fd, int32_t request, int32_t InCount, int32_t OutCont, void *vec);
*/
static int fsa_fd;
int FSA_Init()
{
    if(IOS_Open == NULL)
    {
        OSDynLoad_Module coreinit_handle;
        OSDynLoad_Acquire("coreinit.rpl", &coreinit_handle);
        OSDynLoad_FindExport(coreinit_handle, 0, "IOS_Open", (void**)&IOS_Open);
    }
    fsa_fd = IOS_Open("/dev/fsa", 0);
    if(fsa_fd < 0)
        return fsa_fd;
    return 0;
}

int FSA_Exit()
{
    if(IOS_Close == NULL)
    {
        OSDynLoad_Module coreinit_handle;
        OSDynLoad_Acquire("coreinit.rpl", &coreinit_handle);
        OSDynLoad_FindExport(coreinit_handle, 0, "IOS_Close", (void**)&IOS_Close);
    }
    return IOS_Close(fsa_fd);
}

int svcIoctlv(int fd, int num, int read_cnt, int write_cnt, void* iovec)
{
    if(IOS_Ioctlv == NULL)
    {
        OSDynLoad_Module coreinit_handle;
        OSDynLoad_Acquire("coreinit.rpl", &coreinit_handle);
        OSDynLoad_FindExport(coreinit_handle, 0, "IOS_Ioctlv", (void**)&IOS_Ioctlv);
    }
    return IOS_Ioctlv(fd, 0x10, 2, 1, iovec);
}

int svcIoctl(int32_t fd, int32_t request, void *buffer_in, int32_t bytes_in, void *buffer_io, int32_t bytes_io)
{
    if(IOS_Ioctl == NULL)
    {
        OSDynLoad_Module coreinit_handle;
        OSDynLoad_Acquire("coreinit.rpl", &coreinit_handle);
        OSDynLoad_FindExport(coreinit_handle, 0, "IOS_Ioctl", (void**)&IOS_Ioctl);
    }
    return IOS_Ioctl(fd, request, buffer_in, bytes_in, buffer_io, bytes_io);
}

static void* allocIobuf()
{
	void* ptr = MEM2_alloc(0x828, 0x40);

	memset(ptr, 0x00, 0x828);

	return ptr;
}

static void freeIobuf(void* ptr)
{
	MEM2_free(ptr);
}

int FSA_Mount(char* device_path, char* volume_path, uint32_t flags, char* arg_string, int arg_string_len)
{
	uint8_t* iobuf = allocIobuf();
	uint8_t* inbuf8 = iobuf;
	uint8_t* outbuf8 = &iobuf[0x520];
	iovec_s* iovec = (iovec_s*)&iobuf[0x7C0];
	uint32_t* inbuf = (uint32_t*)inbuf8;
	uint32_t* outbuf = (uint32_t*)outbuf8;

	strncpy((char*)&inbuf8[0x04], device_path, 0x27F);
	strncpy((char*)&inbuf8[0x284], volume_path, 0x27F);
	inbuf[0x504 / 4] = (uint32_t)flags;
	inbuf[0x508 / 4] = (uint32_t)arg_string_len;

	iovec[0].ptr = inbuf;
	iovec[0].len = 0x520;
	iovec[1].ptr = arg_string;
	iovec[1].len = arg_string_len;
	iovec[2].ptr = outbuf;
	iovec[2].len = 0x293;

	int ret = svcIoctlv(fsa_fd, 0x01, 2, 1, iovec);

	freeIobuf(iobuf);
	return ret;
}

int FSA_Unmount(char* path, uint32_t flags)
{
	uint8_t* iobuf = allocIobuf();
	uint32_t* inbuf = (uint32_t*)iobuf;
	uint32_t* outbuf = (uint32_t*)&iobuf[0x520];

	strncpy((char*)&inbuf[0x01], path, 0x27F);
	inbuf[0x284 / 4] = flags;

	int ret = svcIoctl(fsa_fd, 0x02, inbuf, 0x520, outbuf, 0x293);

	freeIobuf(iobuf);
	return ret;
}

int FSA_MakeDir(char* path, uint32_t flags)
{
	uint8_t* iobuf = allocIobuf();
	uint32_t* inbuf = (uint32_t*)iobuf;
	uint32_t* outbuf = (uint32_t*)&iobuf[0x520];

	strncpy((char*)&inbuf[0x01], path, 0x27F);
	inbuf[0x284 / 4] = flags;

	int ret = svcIoctl(fsa_fd, 0x07, inbuf, 0x520, outbuf, 0x293);

	freeIobuf(iobuf);
	return ret;
}

int FSA_OpenDir(char* path, int* outHandle)
{
	uint8_t* iobuf = allocIobuf();
	uint32_t* inbuf = (uint32_t*)iobuf;
	uint32_t* outbuf = (uint32_t*)&iobuf[0x520];

	strncpy((char*)&inbuf[0x01], path, 0x27F);

	int ret = svcIoctl(fsa_fd, 0x0A, inbuf, 0x520, outbuf, 0x293);

	if(outHandle) *outHandle = outbuf[1];

	freeIobuf(iobuf);
	return ret;
}

int FSA_ReadDir(int handle, directoryEntry_s* out_data)
{
	uint8_t* iobuf = allocIobuf();
	uint32_t* inbuf = (uint32_t*)iobuf;
	uint32_t* outbuf = (uint32_t*)&iobuf[0x520];

	inbuf[1] = handle;

	int ret = svcIoctl(fsa_fd, 0x0B, inbuf, 0x520, outbuf, 0x293);

	if(out_data) memcpy(out_data, &outbuf[1], sizeof(directoryEntry_s));

	freeIobuf(iobuf);
	return ret;
}

int FSA_CloseDir(int handle)
{
	uint8_t* iobuf = allocIobuf();
	uint32_t* inbuf = (uint32_t*)iobuf;
	uint32_t* outbuf = (uint32_t*)&iobuf[0x520];

	inbuf[1] = handle;

	int ret = svcIoctl(fsa_fd, 0x0D, inbuf, 0x520, outbuf, 0x293);

	freeIobuf(iobuf);
	return ret;
}

int FSA_OpenFile(char* path, char* mode, int* outHandle)
{
	uint8_t* iobuf = allocIobuf();
	uint32_t* inbuf = (uint32_t*)iobuf;
	uint32_t* outbuf = (uint32_t*)&iobuf[0x520];

	strncpy((char*)&inbuf[0x01], path, 0x27F);
	strncpy((char*)&inbuf[0xA1], mode, 0x10);

	int ret = svcIoctl(fsa_fd, 0x0E, inbuf, 0x520, outbuf, 0x293);

	if(outHandle) *outHandle = outbuf[1];

	freeIobuf(iobuf);
	return ret;
}

int _FSA_ReadWriteFile(void* data, uint32_t size, uint32_t cnt, int fileHandle, uint32_t flags, bool read)
{
	uint8_t* iobuf = allocIobuf();
	uint8_t* inbuf8 = iobuf;
	uint8_t* outbuf8 = &iobuf[0x520];
	iovec_s* iovec = (iovec_s*)&iobuf[0x7C0];
	uint32_t* inbuf = (uint32_t*)inbuf8;
	uint32_t* outbuf = (uint32_t*)outbuf8;

	inbuf[0x08 / 4] = size;
	inbuf[0x0C / 4] = cnt;
	inbuf[0x14 / 4] = fileHandle;
	inbuf[0x18 / 4] = flags;

	iovec[0].ptr = inbuf;
	iovec[0].len = 0x520;

	iovec[1].ptr = data;
	iovec[1].len = size * cnt;

	iovec[2].ptr = outbuf;
	iovec[2].len = 0x293;

	int ret;
	if(read) ret = svcIoctlv(fsa_fd, 0x0F, 1, 2, iovec);
	else ret = svcIoctlv(fsa_fd, 0x10, 2, 1, iovec);

	freeIobuf(iobuf);
	return ret;
}

int FSA_ReadFile(void* data, uint32_t size, uint32_t cnt, int fileHandle, uint32_t flags)
{
	return _FSA_ReadWriteFile(data, size, cnt, fileHandle, flags, true);
}

int FSA_WriteFile(void* data, uint32_t size, uint32_t cnt, int fileHandle, uint32_t flags)
{
	return _FSA_ReadWriteFile(data, size, cnt, fileHandle, flags, false);
}

int FSA_StatFile(int handle, fileStat_s* out_data)
{
	uint8_t* iobuf = allocIobuf();
	uint32_t* inbuf = (uint32_t*)iobuf;
	uint32_t* outbuf = (uint32_t*)&iobuf[0x520];

	inbuf[1] = handle;

	int ret = svcIoctl(fsa_fd, 0x14, inbuf, 0x520, outbuf, 0x293);

	if(out_data) memcpy(out_data, &outbuf[1], sizeof(fileStat_s));

	freeIobuf(iobuf);
	return ret;
}

// int FSA_CloseFile(int fd, int fileHandle)
// {
// 	return _FSA_CloseFile(fd, fileHandle);
// }


int FSA_ChangeMode(char* path, int mode)
{
    uint8_t* iobuf = allocIobuf();
	uint8_t* inbuf8 = iobuf;
	uint8_t* outbuf8 = &iobuf[0x520];
	uint32_t* inbuf = (uint32_t*)inbuf8;
	uint32_t* outbuf = (uint32_t*)outbuf8;

	strncpy((char*)&inbuf8[0x04], path, 0x27F);
	inbuf[0x284 / 4] = mode;
    inbuf[0x288 / 4] = 0x666; // ??

	int ret = svcIoctl(fsa_fd, 0x20, inbuf, 0x520, outbuf, 0x293);

	freeIobuf(iobuf);
	return ret;
}

int FSA_Format(char* device, char* fs_format, int flags, uint32_t what1, uint32_t what2)
{
	uint8_t* iobuf = allocIobuf();
	uint8_t* inbuf8 = iobuf;
	uint8_t* outbuf8 = &iobuf[0x520];
	uint32_t* inbuf = (uint32_t*)inbuf8;
	uint32_t* outbuf = (uint32_t*)outbuf8;

	strncpy((char*)&inbuf8[0x04], device, 0x27F);
	strncpy((char*)&inbuf8[0x284], fs_format, 8);
	inbuf[0x28C / 4] = what1;
	inbuf[0x290 / 4] = what2;

	int ret = svcIoctl(fsa_fd, 0x69, inbuf, 0x520, outbuf, 0x293);

	freeIobuf(iobuf);
	return ret;
}

// type 4 :
// 		0x08 : device size in sectors (uint64_t)
// 		0x10 : device sector size (uint32_t)
int FSA_GetDeviceInfo(char* device_path, int type, uint32_t* out_data)
{
	uint8_t* iobuf = allocIobuf();
	uint32_t* inbuf = (uint32_t*)iobuf;
	uint32_t* outbuf = (uint32_t*)&iobuf[0x520];

	strncpy((char*)&inbuf[0x01], device_path, 0x27F);
	inbuf[0x284 / 4] = type;

	int ret = svcIoctl(fsa_fd, 0x18, inbuf, 0x520, outbuf, 0x293);

	int size = 0;

	switch(type)
	{
		case 0: case 1: case 7:
			size = 0x8;
			break;
		case 2:
			size = 0x4;
			break;
		case 3:
			size = 0x1E;
			break;
		case 4:
			size = 0x28;
			break;
		case 5:
			size = 0x64;
			break;
		case 6: case 8:
			size = 0x14;
			break;
	}

	memcpy(out_data, &outbuf[1], size);

	freeIobuf(iobuf);
	return ret;
}


int FSA_GetVolumeInfo(char* volume_path, int type, fsa_volume_info* out)
{
	uint8_t* iobuf = allocIobuf();
	uint32_t* inbuf = (uint32_t*)iobuf;
	uint32_t* outbuf = (uint32_t*)&iobuf[0x520];

	strncpy((char*)&inbuf[0x01], volume_path, 0x27F);

	int ret = svcIoctl(fsa_fd, 0x03, inbuf, 0x520, outbuf, 0x293);

	memcpy(out, &outbuf[1], 0x1BC);

	freeIobuf(iobuf);
	return ret;
}

int FSA_RawOpen(char* device_path, int* outHandle)
{
	uint8_t* iobuf = allocIobuf();
	uint32_t* inbuf = (uint32_t*)iobuf;
	uint32_t* outbuf = (uint32_t*)&iobuf[0x520];

	strncpy((char*)&inbuf[0x01], device_path, 0x27F);

	int ret = svcIoctl(fsa_fd, 0x6A, inbuf, 0x520, outbuf, 0x293);

	if(outHandle) *outHandle = outbuf[1];

	freeIobuf(iobuf);
	return ret;
}

int FSA_RawClose(int device_handle)
{
	uint8_t* iobuf = allocIobuf();
	uint32_t* inbuf = (uint32_t*)iobuf;
	uint32_t* outbuf = (uint32_t*)&iobuf[0x520];

	inbuf[1] = device_handle;

	int ret = svcIoctl(fsa_fd, 0x6D, inbuf, 0x520, outbuf, 0x293);

	freeIobuf(iobuf);
	return ret;
}

// offset in blocks of 0x1000 bytes
int FSA_RawRead(void* data, uint32_t size_bytes, uint32_t cnt, uint64_t blocks_offset, int device_handle)
{
	uint8_t* iobuf = allocIobuf();
	uint8_t* inbuf8 = iobuf;
	uint8_t* outbuf8 = &iobuf[0x520];
	iovec_s* iovec = (iovec_s*)&iobuf[0x7C0];
	uint32_t* inbuf = (uint32_t*)inbuf8;
	uint32_t* outbuf = (uint32_t*)outbuf8;

	// note : offset_bytes = blocks_offset * size_bytes
	inbuf[0x08 / 4] = (blocks_offset >> 32);
	inbuf[0x0C / 4] = (blocks_offset & 0xFFFFFFFF);
	inbuf[0x10 / 4] = cnt;
	inbuf[0x14 / 4] = size_bytes;
	inbuf[0x18 / 4] = device_handle;

	iovec[0].ptr = inbuf;
	iovec[0].len = 0x520;

	iovec[1].ptr = data;
	iovec[1].len = size_bytes * cnt;

	iovec[2].ptr = outbuf;
	iovec[2].len = 0x293;

	int ret = svcIoctlv(fsa_fd, 0x6B, 1, 2, iovec);

	freeIobuf(iobuf);
	return ret;
}

int FSA_RawWrite(void* data, uint32_t size_bytes, uint32_t cnt, uint64_t blocks_offset, int device_handle)
{
	uint8_t* iobuf = allocIobuf();
	uint8_t* inbuf8 = iobuf;
	uint8_t* outbuf8 = &iobuf[0x520];
	iovec_s* iovec = (iovec_s*)&iobuf[0x7C0];
	uint32_t* inbuf = (uint32_t*)inbuf8;
	uint32_t* outbuf = (uint32_t*)outbuf8;

	inbuf[0x08 / 4] = (blocks_offset >> 32);
	inbuf[0x0C / 4] = (blocks_offset & 0xFFFFFFFF);
	inbuf[0x10 / 4] = cnt;
	inbuf[0x14 / 4] = size_bytes;
	inbuf[0x18 / 4] = device_handle;

	iovec[0].ptr = inbuf;
	iovec[0].len = 0x520;

	iovec[1].ptr = data;
	iovec[1].len = size_bytes * cnt;

	iovec[2].ptr = outbuf;
	iovec[2].len = 0x293;

	int ret = svcIoctlv(fsa_fd, 0x6C, 2, 1, iovec);

	freeIobuf(iobuf);
	return ret;
}
