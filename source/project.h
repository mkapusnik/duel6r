/*
* Copyright (c) 2006, Ondrej Danek (www.ondrej-danek.net)
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Ondrej Danek nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
* GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef DUEL6_PROJECT_H
#define DUEL6_PROJECT_H

#include <string>
#include "core/co_core.h"
#include "mylib/mycam.h"
#include "sound/sound.h"
#include "Globals.h"
#include "Type.h"
#include "ScreenMode.h"
#include "FaceList.h"
#include "World.h"
#include "InfoMessageQueue.h"

namespace Duel6
{
	//////////////////////////////////////////////////////////////////////
	//                          render.cpp                              //
	//////////////////////////////////////////////////////////////////////
	void RENDER_SetGLMode(Int32 mode);
	void RENDER_InitScreen();
	void RENDER_DrawScene(const Game& game);

	//////////////////////////////////////////////////////////////////////
	//                          explode.cpp                             //
	//////////////////////////////////////////////////////////////////////
	void    EXPL_Init();
	void    EXPL_MoveAll(Float32 elapsedTime);
	void    EXPL_DrawAll();
	void    EXPL_Add(Float32 x, Float32 y, Float32 s, Float32 m, const Color& color);

	//////////////////////////////////////////////////////////////////////
	//                          fire.cpp                                //
	//////////////////////////////////////////////////////////////////////
	void    FIRE_Init();
	void    FIRE_Find(FaceList& sprites);
	void    FIRE_Check(Float32 X, Float32 Y, Float32 d);
}

#endif
