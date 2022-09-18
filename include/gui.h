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

#include <wut.h>
#include <memory.h>
#include <gx2/texture.h>
#include <vpad/input.h>

#include "draw.h"
#include "font.h"
#include "texture.h"

enum available_states
{
    STATE_LOADING = 0,
    STATE_MAIN_MENU,
    STATE_INSTALL_MENU,
    STATE_FILEBROWSE_MENU,
    STATE_TITLEMANAGE_MENU,
};

#define NEUTRAL_COLOR 0.5f, 0.5f, 0.5f, 1.0f
#define INSTALL_COLOR 0.192f, 0.404f, 0.737f, 1.0f

//Common GUI Textures
extern GX2Texture texture;
extern GX2Texture textureBackdrop;
extern GX2Texture textureHeader;
extern GX2Texture textureInfoHeader;
extern GX2Texture textureContent;
extern GX2Texture textureContentSubheader;
extern GX2Texture textureBullet;
extern GX2Texture textureFolder;
extern GX2Texture textureFSTFolder;
extern GX2Texture textureYButton;
extern GX2Texture textureIconBack;
extern GX2Texture textureProgressBar;
extern GX2Texture textureDRCArrow;
extern GX2Texture textureDRCButton;
extern GX2Texture textureDRCInstall;
extern GX2Texture textureDRCShadow;
extern GX2Texture textureDRCMeme;
extern GX2Texture textureDRCCornerButton;
extern GX2Texture textureDRCBackImage;
extern GX2Texture textureDRCTitleButton;
extern GX2Texture fontTexture;

//State variables
extern VPADStatus vpad;
extern float tpXPos;
extern float tpYPos;
extern bool tpTouched;
extern bool app_is_running;

extern int current_state;

void gui_init();
void gui_transition_update();
void gui_transition_in(float start_scale, float start_xshift, float start_yshift, float frames);
void gui_transition_out(float end_scale, float end_xshift, float end_yshift, float frames);
bool gui_transition_done();
