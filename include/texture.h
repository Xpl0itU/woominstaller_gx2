/*
 *  Woomïnstaller GX2 - Homebrew package installer for Wii U
 *
 *  Copyright (C) 2016          SALT
 *  Copyright (C) 2016          Max Thomas (Shiny Quagsire) <mtinc2@gmail.com>
 *  Copyright (C) 2016          WulfyStylez
 *
 *  This code is licensed under the terms of the GNU LGPL, version 2.1
 *  see file LICENSE.md or https://www.gnu.org/licenses/lgpl-2.1.txt
 */

#pragma once

#include <wut_types.h>
#include <coreinit/debug.h>

#include <stdio.h>

#include <gx2/texture.h>
#include <gx2/mem.h>
#include <gx2/enum.h>
#include <gx2/shaders.h>
#include "gx2_ext.h"

#include "memory.h"

typedef struct tga_hdr tga_hdr;
struct __attribute__((__packed__)) tga_hdr
{
    uint8_t idlength;
    uint8_t colormaptype;
    uint8_t datatype;
    uint16_t colormaporigin;
    uint16_t colormaplength;
    uint8_t colormapdepth;
    uint16_t x_origin;
    uint16_t y_origin;
    uint16_t width;
    uint16_t height;
    uint8_t bpp;
    uint8_t imagedescriptor;
};

void load_tga(GX2Texture *texture, void* img_data);
bool load_img_texture(GX2Texture *texture, char *path);
bool load_img_texture_mask(GX2Texture *texture, char *path, char *mask);
void free_img_texture(GX2Texture *texture);
