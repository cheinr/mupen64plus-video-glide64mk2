/******************************************************************************
 * Glide64 - Glide video plugin for Nintendo 64 emulators.
 * http://bitbucket.org/wahrhaft/mupen64plus-video-glide64/
 *
 * Copyright (C) 2010 Jon Ring
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *****************************************************************************/

#ifndef M64P_H
#define M64P_H

#ifdef M64P_STATIC_PLUGINS
#define M64P_CORE_PROTOTYPES 1
#endif

#include "m64p_types.h"
#include "m64p_config.h"
#include "m64p_vidext.h"


#if (!M64P_STATIC_PLUGINS)
extern ptr_ConfigOpenSection      ConfigOpenSection;
extern ptr_ConfigGetParamInt      ConfigGetParamInt;
extern ptr_ConfigGetParamBool     ConfigGetParamBool;

extern ptr_VidExt_Init                  CoreVideo_Init;
extern ptr_VidExt_Quit                  CoreVideo_Quit;
extern ptr_VidExt_ListFullscreenModes   CoreVideo_ListFullscreenModes;
extern ptr_VidExt_SetVideoMode          CoreVideo_SetVideoMode;
extern ptr_VidExt_SetCaption            CoreVideo_SetCaption;
extern ptr_VidExt_ToggleFullScreen      CoreVideo_ToggleFullScreen;
extern ptr_VidExt_GL_GetProcAddress     CoreVideo_GL_GetProcAddress;
extern ptr_VidExt_GL_SetAttribute       CoreVideo_GL_SetAttribute;
extern ptr_VidExt_GL_SwapBuffers        CoreVideo_GL_SwapBuffers;
#else

#define CoreVideo_Init VidExt_Init
#define CoreVideo_Quit VidExt_Quit
#define CoreVideo_ListFullscreenModes VidExt_ListFullscreenModes
#define CoreVideo_SetVideoMode VidExt_SetVideoMode
#define CoreVideo_SetCaption VidExt_SetCaption
#define CoreVideo_ToggleFullScreen VidExt_ToggleFullScreen
#define CoreVideo_GL_GetProcAddress VidExt_GL_GetProcAddress
#define CoreVideo_GL_SetAttribute VidExt_GL_SetAttribute
#define CoreVideo_GL_SwapBuffers VidExt_GL_SwapBuffers

#endif

#endif
