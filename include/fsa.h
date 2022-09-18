/*
 *  Woomïnstaller GX2 - Homebrew package installer for Wii U
 *
 *  Copyright (C) 2016          SALT
 *  Copyright (C) 2016          WulfyStylez
 *
 *  This code is licensed under the terms of the GNU LGPL, version 2.1
 *  see file LICENSE.md or https://www.gnu.org/licenses/lgpl-2.1.txt
 */

#pragma once

#include "common.h"

typedef struct
{
    uint32_t flags;      // 0x0
    uint32_t permissions;// 0x4
	uint32_t unk1[0x2];  // 0x8
	uint32_t size;       // 0x10 size in bytes
	uint32_t physsize;   // 0x14 physical size on disk in bytes
	uint32_t unk2[0x13];
}fileStat_s;

typedef struct
{
	fileStat_s dirStat;
	char name[0x100];
}directoryEntry_s;

typedef enum
{
    MEDIA_READY = 0,
    MEDIA_NOT_PRESENT = 1,
    MEDIA_INVALID = 2,
    MEDIA_DIRTY = 3,
    MEDIA_FATAL = 4
} media_states;

typedef struct
{
    uint32_t flags;
    uint32_t media_state;
    int32_t device_index;
    uint32_t block_size;
    uint32_t logical_block_size;
    uint32_t read_align;
    uint32_t write_align;
    char dev_type[8]; // sdcard, slc, mlc, etc
    char fs_type[8]; // isfs, wfs, fat, etc
    char vol_label[0x80];
    char vol_id[0x80];
    char dev_node[0x10];
    char mount_path[0x80];
} fsa_volume_info;

#define FSA_MOUNTFLAGS_BINDMOUNT (1 << 0)
#define FSA_MOUNTFLAGS_GLOBAL (1 << 1)

int FSA_Init();
int FSA_Exit();

int FSA_Mount(char* device_path, char* volume_path, uint32_t flags, char* arg_string, int arg_string_len);
int FSA_Unmount(char* path, uint32_t flags);

int FSA_GetDeviceInfo(char* device_path, int type, uint32_t* out_data);
int FSA_GetVolumeInfo(char* volume_path, int type, fsa_volume_info* out_data);

int FSA_MakeDir(char* path, uint32_t flags);
int FSA_OpenDir(char* path, int* outHandle);
int FSA_ReadDir(int handle, directoryEntry_s* out_data);
int FSA_CloseDir(int handle);

int FSA_OpenFile(char* path, char* mode, int* outHandle);
int FSA_ReadFile(void* data, uint32_t size, uint32_t cnt, int fileHandle, uint32_t flags);
int FSA_WriteFile(void* data, uint32_t size, uint32_t cnt, int fileHandle, uint32_t flags);
int FSA_StatFile(int handle, fileStat_s* out_data);
int FSA_CloseFile(int fileHandle);

int FSA_ChangeMode(char* path, int mode);

int FSA_Format(char* device, char* fs_format, int flags, uint32_t what1, uint32_t what2);

int FSA_RawOpen(char* device_path, int* outHandle);
int FSA_RawRead(void* data, uint32_t size_bytes, uint32_t cnt, uint64_t sector_offset, int device_handle);
int FSA_RawWrite(void* data, uint32_t size_bytes, uint32_t cnt, uint64_t sector_offset, int device_handle);
int FSA_RawClose(int device_handle);
