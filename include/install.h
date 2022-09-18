/*
 *  Woomïnstaller GX2 - Homebrew package installer for Wii U
 *
 *  Copyright (C) 2016          SALT
 *  Copyright (C) 2016          Max Thomas (Shiny Quagsire) <mtinc2@gmail.com>
 *
 *  This code is licensed under the terms of the GNU LGPL, version 2.1
 *  see file LICENSE.md or https://www.gnu.org/licenses/lgpl-2.1.txt
 */

#pragma once

#include <coreinit/core.h>
#include <coreinit/debug.h>
#include <coreinit/thread.h>
#include <coreinit/filesystem.h>
#include <coreinit/foreground.h>
#include <coreinit/screen.h>
#include <coreinit/internal.h>
#include <coreinit/mcp.h>

#include <dirent.h>
#include <stdio.h>
#include <string.h>

#include "gui.h"
#include "miniz.h"
#include "ezxml.h"

#define INSTALL_QUEUE_SIZE 0x100

typedef struct MCPDeviceList {
    MCPDevice devices[32];
} MCPDeviceList;

typedef struct InstallQueueItem
{
    char *targetPath;
    char *archiveName;
    bool pre_install;
    bool installing;
    bool woomy;
    float percent_complete;
    uint32_t current_unpack;
    uint32_t total_unpack;
    uint32_t current_content;
    uint32_t total_content;
    uint8_t targetDevice;
    GX2Texture *texture;
} InstallQueueItem;

typedef struct InstallDevice
{
    int deviceID;
    int deviceNum;
    char *deviceName;
} InstallDevice;

void install_shift_back_queue();
void install_add_to_queue(char *path);
void install_init_start_queue_manager();
void install_deinit();

int install_get_target();
void install_cycle_device();
InstallDevice *install_get_device(uint8_t device_id);
void install_abort_current();
InstallQueueItem *install_get_queue_item(int index);
char *install_get_queue_path(int index);
char *install_get_current_install_name();
uint8_t install_get_queue_install_device(int index);
GX2Texture *install_get_queue_texture(int index);