// ==========================================================================
//
// GenPoint.cpp			JIT Class for 3D Rendering Library
//
//						This file contains the rasterizer functions that
//						implement the runtime code generation support
//						for optimized scan line rasterization routines.
//
// --------------------------------------------------------------------------
//
// 12-29-2003		Hans-Martin Will	initial version
//
// --------------------------------------------------------------------------
//
// Copyright (c) 2004, Hans-Martin Will. All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are 
// met:
// 
//	 *  Redistributions of source code must retain the above copyright
// 		notice, this list of conditions and the following disclaimer. 
//   *	Redistributions in binary form must reproduce the above copyright
// 		notice, this list of conditions and the following disclaimer in the 
// 		documentation and/or other materials provided with the distribution. 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
// OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
// THE POSSIBILITY OF SUCH DAMAGE.
//
// ==========================================================================


#include "stdafx.h"
#include "CodeGenerator.h"
#include "Rasterizer.h"
#include "FunctionCache.h"
#include "Surface.h"
#include "Texture.h"
#include "codegen.h"
#include "instruction.h"
#include "emit.h"
#include "arm-dis.h"


using namespace EGL;


namespace {

	inline cg_virtual_reg_t * LOAD_DATA(cg_block_t * block, cg_virtual_reg_t * base, I32 constant) {
		cg_virtual_reg_t * offset = cg_virtual_reg_create(block->proc, cg_reg_type_general);
		cg_virtual_reg_t * addr = cg_virtual_reg_create(block->proc, cg_reg_type_general);
		cg_virtual_reg_t * value = cg_virtual_reg_create(block->proc, cg_reg_type_general);

		LDI(offset, constant);
		ADD(addr, base, offset);
		LDW(value, addr);

		return value;
	}

#define ALLOC_REG(reg) reg = cg_virtual_reg_create(procedure, cg_reg_type_general)
#define ALLOC_FLAGS(reg) reg = cg_virtual_reg_create(procedure, cg_reg_type_flags)
#define DECL_REG(reg) cg_virtual_reg_t * reg = cg_virtual_reg_create(procedure, cg_reg_type_general)
#define DECL_FLAGS(reg) cg_virtual_reg_t * reg = cg_virtual_reg_create(procedure, cg_reg_type_flags)
#define DECL_CONST_REG(reg, value) cg_virtual_reg_t * reg = cg_virtual_reg_create(procedure, cg_reg_type_general); LDI(reg, value)

	U32 Log(U32 value) {
		U32 result = 0;
		U32 mask = 1;

		while ((value & mask) != value) {
			++result;
			mask = (mask << 1) | 1;
		}

		return result;
	}
}


void CodeGenerator :: GenerateRasterPoint() {

	cg_proc_t * procedure = cg_proc_create(m_Module);

	// The signature of the generated function is:
	//	(const RasterInfo * info, const RasterPos& pos);

	DECL_REG	(regInfo);		// virtual register containing info structure pointer
	DECL_REG	(regPos);		// virtual register containing vertex coordinate pointer
	DECL_REG	(regSize);		// virtual register containing point size

	procedure->num_args = 3;	// the previous three declarations make up the arguments

	cg_block_t * block = cg_block_create(procedure, 1);

	// load argument values
	cg_virtual_reg_t * regTexture = LOAD_DATA(block, regInfo, OFFSET_TEXTURES);

	FragmentGenerationInfo info;
	size_t unit;

	info.regInfo = regInfo;

	info.regTexture[0] = regTexture;

	for (unit = 1; unit < EGL_NUM_TEXTURE_UNITS; ++unit) {
		DECL_CONST_REG	(regOffset, unit * 4);
		DECL_REG		(regTextureN);

		ADD				(regTextureN, regTexture, regOffset);

		info.regTexture[unit] = regTextureN;
	}

	// EGL_Fixed halfSize = size / 2;
	DECL_REG		(regHalfSize);
	DECL_CONST_REG	(regConstant1, 1);

	ASR				(regHalfSize, regSize, regConstant1);

	// I32 xmin = EGL_IntFromFixed(point.m_WindowCoords.x - halfSize + EGL_HALF);
	// I32 xmax = xmin + EGL_IntFromFixed(size) - 1;
	// I32 ymin = EGL_IntFromFixed(point.m_WindowCoords.y - halfSize + EGL_HALF);
	// I32 ymax = ymin + EGL_IntFromFixed(size) - 1;

	DECL_CONST_REG	(regHalf, 0x8000);
	DECL_REG		(regAdjustedSize);
	DECL_REG		(regRoundedSize);

	FSUB			(regAdjustedSize, regSize, regHalf);
	TRUNC			(regRoundedSize, regAdjustedSize);

	DECL_REG		(regWindowXPlus);
	DECL_REG		(regWindowYPlus);
	DECL_REG		(regWindowXPlusSized);
	DECL_REG		(regWindowYPlusSized);

	DECL_REG		(regXMin);
	DECL_REG		(regXMax);
	DECL_REG		(regYMin);
	DECL_REG		(regYMax);

	cg_virtual_reg_t *	regPointWindowX = LOAD_DATA(block, regPos, OFFSET_RASTER_POS_WINDOW_X);

	FADD			(regWindowXPlus, regPointWindowX, regHalf);
	FSUB			(regWindowXPlusSized, regWindowXPlus, regHalfSize);
	TRUNC			(regXMin, regWindowXPlusSized);

	ADD				(regXMax, regXMin, regRoundedSize);

	cg_virtual_reg_t *	regPointWindowY = LOAD_DATA(block, regPos, OFFSET_RASTER_POS_WINDOW_Y);

	FADD			(regWindowYPlus, regPointWindowY, regHalf);
	FSUB			(regWindowYPlusSized, regWindowYPlus, regHalfSize);
	TRUNC			(regYMin, regWindowYPlusSized);

	ADD				(regYMax, regYMin, regRoundedSize);

	// EGL_Fixed depth = point.m_WindowCoords.depth;
	// FractionalColor baseColor = point.m_Color;
	// EGL_Fixed fogDensity = point.m_FogDensity;

	info.regFog = LOAD_DATA(block, regPos, OFFSET_RASTER_POS_FOG);
	info.regDepth = LOAD_DATA(block, regPos, OFFSET_RASTER_POS_WINDOW_DEPTH);
	info.regR = LOAD_DATA(block, regPos, OFFSET_RASTER_POS_COLOR_R);
	info.regG = LOAD_DATA(block, regPos, OFFSET_RASTER_POS_COLOR_G);
	info.regB = LOAD_DATA(block, regPos, OFFSET_RASTER_POS_COLOR_B);
	info.regA = LOAD_DATA(block, regPos, OFFSET_RASTER_POS_COLOR_A);

	cg_block_ref_t * blockEndProc = cg_block_ref_create(procedure);

	if (!m_State->m_Point.SpriteEnabled && !m_State->m_Point.CoordReplaceEnabled) {
		//	EGL_Fixed tu = point.m_TextureCoords.tu;
		//	EGL_Fixed tv = point.m_TextureCoords.tv;

		for (unit = 0; unit < EGL_NUM_TEXTURE_UNITS; ++unit) {
			info.regU[unit] = LOAD_DATA(block, regPos, OFFSET_RASTER_POS_TEX_TU + unit * sizeof(TexCoord));
			info.regV[unit] = LOAD_DATA(block, regPos, OFFSET_RASTER_POS_TEX_TV + unit * sizeof(TexCoord)); 
		}

		//	for (I32 y = ymin; y <= ymax; y++) {
		block = cg_block_create(procedure, 3);
		cg_block_ref_t * loopYBegin = cg_block_ref_create(procedure);
		loopYBegin->block = block;

		DECL_REG	(regYLoopEnter);
		DECL_REG	(regYLoopExit);

		PHI			(regYLoopEnter, cg_create_virtual_reg_list(procedure->module->heap, regYLoopExit, regYMin, NULL));

		info.regY =	regYLoopEnter;

		DECL_REG	(regInitX);
		OR			(regInitX, regXMin, regXMin);

		//		for (I32 x = xmin; x <= xmax; x++) {
		block = cg_block_create(procedure, 9);
		cg_block_ref_t * loopXBegin = cg_block_ref_create(procedure);
		loopXBegin->block = block;

		DECL_REG	(regXLoopEnter);
		DECL_REG	(regXLoopExit);

		PHI			(regXLoopEnter, cg_create_virtual_reg_list(procedure->module->heap, regXLoopExit, regInitX, NULL));

		info.regX =	regXLoopEnter;

		//			Fragment(x, y, depth, tu, tv, fogDensity, baseColor);

		cg_block_ref_t * postFragment = cg_block_ref_create(procedure);

		GenerateFragment(procedure, block, postFragment, info, 9, true);

		block = cg_block_create(procedure, 9);
		postFragment->block = block;

		FADD		(regXLoopExit,	regXLoopEnter, regConstant1);
		DECL_FLAGS	(flagsXLimit);

		FCMP		(flagsXLimit, regXLoopExit, regXMax);
		BLE			(flagsXLimit, loopXBegin);
		//		}

		block = cg_block_create(procedure, 3);

		FADD		(regYLoopExit,	regYLoopEnter, regConstant1);
		DECL_FLAGS	(flagsYLimit);

		FCMP		(flagsYLimit, regYLoopExit, regYMax);
		BLE			(flagsYLimit, loopYBegin);
		//BRA			(blockEndProc);
		//	}
	} else {

		//	EGL_Fixed delta = EGL_Inverse(size);
		DECL_REG	(regDelta0);
		DECL_REG	(regDelta);
		DECL_REG	(regDeltaHalf);

		FINV		(regDelta0, regSize);
		OR			(regDelta, regDelta0, regDelta0);

		for (unit = 0; unit < EGL_NUM_TEXTURE_UNITS; ++unit) {
			if (m_State->m_Texture[unit].MipmapFilterMode != RasterizerState::FilterModeNone) {

				if (m_State->m_Texture[unit].MipmapFilterMode == RasterizerState::FilterModeNearest ||
					/* remove this */ m_State->m_Texture[unit].MipmapFilterMode == RasterizerState::FilterModeLinear) {
					//EGL_Fixed maxDu = delta >> (16 - m_Texture->GetTexture(0)->GetLogWidth());
					//EGL_Fixed maxDv = delta >> (16 - m_Texture->GetTexture(0)->GetLogHeight());

					cg_virtual_reg_t * regLogWidth = LOAD_DATA(block, info.regTexture[unit], OFFSET_TEXTURE_LOG_WIDTH);
					DECL_CONST_REG	(regConstant16, 16);

					DECL_REG	(regShiftDu);
					DECL_REG	(regShiftDv);
					DECL_REG	(regMaxDu);
					DECL_REG	(regMaxDv);

					SUB			(regShiftDu, regConstant16, regLogWidth);
					ASR			(regMaxDu, regDelta, regShiftDu);

					cg_virtual_reg_t * regLogHeight = LOAD_DATA(block, info.regTexture[unit], OFFSET_TEXTURE_LOG_HEIGHT);
					SUB			(regShiftDv, regConstant16, regLogHeight);
					ASR			(regMaxDv, regDelta, regShiftDv);
					
					//	EGL_Fixed rho = maxDu + maxDv;

					DECL_REG	(regRho);
					FADD		(regRho, regMaxDu, regMaxDv);

					// we start with nearest/minification only selection; will add LINEAR later
					//	m_MipMapLevel = EGL_Min(EGL_Max(0, Log2(rho)), m_MaxMipmapLevel);

					DECL_REG	(regLog2);
					DECL_REG	(regMipmapLevel);

					LOG2		(regLog2, regRho);
					
					cg_virtual_reg_t * regMaxMipmapLevel = LOAD_DATA(block, regInfo, OFFSET_MAX_MIPMAP_LEVEL + unit * sizeof(I32));
					
					MIN			(regMipmapLevel, regLog2, regMaxMipmapLevel);

					// now multiply the texture block size by the mipmap level and add this to the texture base

					assert		((1 << Log(sizeof(Texture))) == sizeof(Texture));

					DECL_CONST_REG	(regLogTextureSize, Log(sizeof(Texture)));
					DECL_REG	(regScaledLevel);

					LSL			(regScaledLevel, regMipmapLevel, regLogTextureSize);
					ALLOC_REG	(regTexture);
					ADD			(regTexture, info.regTexture[unit], regScaledLevel);
					info.regTexture[unit] = regTexture;
				}
			}
		}

		ASR			(regDeltaHalf, regDelta, regConstant1);
		DECL_REG	(regInitV);
		FSUB		(regInitV, regDeltaHalf, regConstant1);

		//	for (I32 y = ymin, tv = delta / 2; y <= ymax; y++, tv += delta) {
		block = cg_block_create(procedure, 3);
		cg_block_ref_t * loopYBegin = cg_block_ref_create(procedure);
		loopYBegin->block = block;

		DECL_REG	(regYLoopEnter);
		DECL_REG	(regYLoopExit);
		DECL_REG	(regVLoopEnter);
		DECL_REG	(regVLoopExit);

		PHI			(regYLoopEnter, cg_create_virtual_reg_list(procedure->module->heap, regYLoopExit, regYMin, NULL));
		PHI			(regVLoopEnter, cg_create_virtual_reg_list(procedure->module->heap, regVLoopExit, regInitV, NULL));

		info.regY =	regYLoopEnter;

		//		for (I32 x = xmin, tu = delta / 2; x <= xmax; x++, tu += delta) {
		DECL_REG	(regInitX);
		OR			(regInitX, regXMin, regXMin);
		DECL_REG	(regInitU);
		FSUB		(regInitU, regDeltaHalf, regConstant1);

		block = cg_block_create(procedure, 9);
		cg_block_ref_t * loopXBegin = cg_block_ref_create(procedure);
		loopXBegin->block = block;

		DECL_REG	(regXLoopEnter);
		DECL_REG	(regXLoopExit);
		DECL_REG	(regULoopEnter);
		DECL_REG	(regULoopExit);

		PHI			(regXLoopEnter, cg_create_virtual_reg_list(procedure->module->heap, regXLoopExit, regInitX, NULL));
		PHI			(regULoopEnter, cg_create_virtual_reg_list(procedure->module->heap, regULoopExit, regInitU, NULL));

		info.regX =	regXLoopEnter;

		for (unit = 0; unit < EGL_NUM_TEXTURE_UNITS; ++unit) {
			info.regU[unit] =	regULoopEnter;
			info.regV[unit] = regVLoopEnter;
		}

		//			Fragment(x, y, depth, tu, tv, fogDensity, baseColor);

		cg_block_ref_t * postFragment = cg_block_ref_create(procedure);

		GenerateFragment(procedure, block, postFragment, info, 9, true);

		block = cg_block_create(procedure, 9);
		postFragment->block = block;

		FADD		(regULoopExit,  regULoopEnter, regDelta);
		FADD		(regXLoopExit,	regXLoopEnter, regConstant1);
		DECL_FLAGS	(flagsXLimit);

		FCMP		(flagsXLimit, regXLoopExit, regXMax);
		BLE			(flagsXLimit, loopXBegin);
		//		}

		block = cg_block_create(procedure, 3);

		FADD		(regVLoopExit,  regVLoopEnter, regDelta);
		FADD		(regYLoopExit,	regYLoopEnter, regConstant1);
		DECL_FLAGS	(flagsYLimit);

		FCMP		(flagsYLimit, regYLoopExit, regYMax);
		BLE			(flagsYLimit, loopYBegin);
	}

	block = cg_block_create(procedure, 1);
	blockEndProc->block = block;

	RET();
}
