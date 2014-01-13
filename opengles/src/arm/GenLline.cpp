// ==========================================================================
//
// GenLine.cpp			JIT Class for 3D Rendering Library
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




void CodeGenerator :: GenerateRasterLine() {

	cg_proc_t * procedure = cg_proc_create(m_Module);

	// The signature of the generated function is:
	//	(const RasterInfo * info, const EdgePos& start, const EdgePos& end);
	// Do not pass in y coordinate but rather assume that raster info pointers have been
	// adjusted to point to current scanline in memory
	// In the edge buffers, z, tu and tv are actually divided by w

	DECL_REG	(regInfo);		// virtual register containing info structure pointer
	DECL_REG	(regFrom);		// virtual register containing start RasterPos pointer
	DECL_REG	(regTo);		// virtual register containing end RasterPos pointer

	procedure->num_args = 3;	// the previous three declarations make up the arguments

	cg_block_t * block = cg_block_create(procedure, 1);
	cg_block_ref_t * blockRefEndProc = cg_block_ref_create(procedure);

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

	// EGL_Fixed deltaX = p_to.m_WindowCoords.x - p_from.m_WindowCoords.x;
	// EGL_Fixed deltaY = p_to.m_WindowCoords.y - p_from.m_WindowCoords.y;
	DECL_REG	(regDeltaX);
	DECL_REG	(regDeltaY);

	cg_virtual_reg_t *	regToX		= LOAD_DATA(block, regTo, OFFSET_RASTER_POS_WINDOW_X);
	cg_virtual_reg_t *	regFromX	= LOAD_DATA(block, regFrom, OFFSET_RASTER_POS_WINDOW_X);
	cg_virtual_reg_t *	regToY		= LOAD_DATA(block, regTo, OFFSET_RASTER_POS_WINDOW_Y);
	cg_virtual_reg_t *	regFromY	= LOAD_DATA(block, regFrom, OFFSET_RASTER_POS_WINDOW_Y);

	FSUB		(regDeltaX, regToX, regFromX);
	FSUB		(regDeltaY, regToY, regFromY);

	DECL_REG	(regAbsDeltaX);
	DECL_REG	(regAbsDeltaY);

	ABS			(regAbsDeltaX, regDeltaX);
	ABS			(regAbsDeltaY, regDeltaY);

	// if (EGL_Abs(deltaX) > EGL_Abs(deltaY)) {

	DECL_FLAGS	(regCompareXY);
	FCMP		(regCompareXY, regAbsDeltaY, regAbsDeltaX);

	cg_block_ref_t * blockRefRasterY = cg_block_ref_create(procedure);

	BGT			(regCompareXY, blockRefRasterY);

		// Bresenham along x-axis
	block = cg_block_create(procedure, 1);

	{
		// 	const RasterPos *start, *end;

		// 	I32 x;
		// 	I32 endX;
		// 	EGL_Fixed roundedX;

		// 	if (deltaX < 0) {
		DECL_FLAGS		(regSignX);
		DECL_CONST_REG	(regZero, 0);
		
		FCMP		(regSignX, regDeltaX, regZero);

		cg_block_ref_t * blockRefPositiveDeltaX = cg_block_ref_create(procedure);

		BGE			(regSignX, blockRefPositiveDeltaX);

		block = cg_block_create(procedure, 1);

		// 		deltaY = -deltaY;
		// 		deltaX = -deltaX;
		DECL_REG	(regMinusDeltaX);
		DECL_REG	(regMinusDeltaY);

		FNEG		(regMinusDeltaX, regDeltaX);
		FNEG		(regMinusDeltaY, regDeltaY);

		// 		start = &p_to;
		// 		end = &p_from;
		DECL_REG	(regStart0);
		DECL_REG	(regEnd0);

		OR			(regStart0, regTo, regTo);
		OR			(regEnd0, regFrom, regFrom);

		// 		roundedX = EGL_NearestInt(p_to.m_WindowCoords.x + 1);
		// 		x = EGL_IntFromFixed(roundedX);
		// 		endX = EGL_IntFromFixed(p_from.m_WindowCoords.x + ((EGL_ONE)/2));
		DECL_REG	(regX0);
		DECL_REG	(regEndX0);
		DECL_CONST_REG	(regHalf, 0x8000);
		DECL_REG	(regXToPlusHalf0);
		DECL_REG	(regXFromPlusHalf0);

		FADD		(regXToPlusHalf0, regToX, regHalf);
		TRUNC		(regX0, regXToPlusHalf0);
		FADD		(regXFromPlusHalf0, regFromX, regHalf);
		TRUNC		(regEndX0, regXFromPlusHalf0);

		cg_block_ref_t * blockRefPostDeltaX = cg_block_ref_create(procedure);

		BRA			(blockRefPostDeltaX);

		// 	} else {
		block = cg_block_create(procedure, 1);
		blockRefPositiveDeltaX->block = block;

		// 		start = &p_from;
		// 		end = &p_to;
		DECL_REG	(regStart1);
		DECL_REG	(regEnd1);

		OR			(regStart1, regFrom, regFrom);
		OR			(regEnd1, regTo, regTo);

		// 		roundedX = EGL_NearestInt(p_from.m_WindowCoords.x);
		// 		x = EGL_IntFromFixed(roundedX);
		// 		endX = EGL_IntFromFixed(p_to.m_WindowCoords.x + ((EGL_ONE)/2-1));
		DECL_REG	(regX1);
		DECL_REG	(regEndX1);
		DECL_CONST_REG	(regHalf1, 0x7fff);
		DECL_REG	(regXToPlusHalf1);
		DECL_REG	(regXFromPlusHalf1);

		FADD		(regXToPlusHalf1, regToX, regHalf1);
		TRUNC		(regEndX1, regXToPlusHalf1);
		FADD		(regXFromPlusHalf1, regFromX, regHalf1);
		TRUNC		(regX1, regXFromPlusHalf1);

		// 	}

		block = cg_block_create(procedure, 1);
		blockRefPostDeltaX->block = block;

		DECL_REG	(regCommonDeltaX);
		DECL_REG	(regCommonDeltaY);

		PHI			(regCommonDeltaX, cg_create_virtual_reg_list(procedure->module->heap, regDeltaX, regMinusDeltaX, NULL));
		PHI			(regCommonDeltaY, cg_create_virtual_reg_list(procedure->module->heap, regDeltaY, regMinusDeltaY, NULL));

		DECL_REG	(regCommonX);
		DECL_REG	(regCommonEndX);

		PHI			(regCommonX, cg_create_virtual_reg_list(procedure->module->heap, regX0, regX1, NULL));
		PHI			(regCommonEndX, cg_create_virtual_reg_list(procedure->module->heap, regEndX0, regEndX1, NULL));

		// 	const RasterPos& from = *start;
		// 	const RasterPos& to = *end;

		DECL_REG	(regCommonFrom);
		DECL_REG	(regCommonTo);

		PHI			(regCommonFrom, cg_create_virtual_reg_list(procedure->module->heap, regStart0, regStart1, NULL));
		PHI			(regCommonTo, cg_create_virtual_reg_list(procedure->module->heap, regEnd0, regEnd1, NULL));

		// 	I32 yIncrement = (deltaY > 0) ? 1 : -1;
		DECL_FLAGS		(regSignY);
		
		FCMP		(regSignY, regCommonDeltaY, regZero);

		cg_block_ref_t * blockRefPositiveDeltaY = cg_block_ref_create(procedure);

		BGT			(regSignY, blockRefPositiveDeltaY);

		block = cg_block_create(procedure, 1);

		DECL_CONST_REG	(regYIncrementNeg, -1);

		cg_block_ref_t * blockRefCommonDeltaY = cg_block_ref_create(procedure);

		BRA			(blockRefCommonDeltaY);

		block = cg_block_create(procedure, 1);
		blockRefPositiveDeltaY->block = block;

		DECL_CONST_REG	(regYIncrementPos, 1);

		block = cg_block_create(procedure, 1);
		blockRefCommonDeltaY->block = block;

		DECL_REG		(regYIncrement);
		PHI				(regYIncrement, cg_create_virtual_reg_list(procedure->module->heap, regYIncrementPos, regYIncrementNeg, NULL));

			// -- initialize with starting vertex attributes
		// 	FractionalColor baseColor = from.m_Color;
		// 	EGL_Fixed OneOverZ = from.m_WindowCoords.invZ;
		// 	EGL_Fixed tuOverZ = EGL_Mul(from.m_TextureCoords.tu, OneOverZ);
		// 	EGL_Fixed tvOverZ = EGL_Mul(from.m_TextureCoords.tv, OneOverZ);
		// 	EGL_Fixed fogDensity = from.m_FogDensity;
		// 	EGL_Fixed depth = from.m_WindowCoords.depth;

		cg_virtual_reg_t * regColorR0 = LOAD_DATA(block, regCommonFrom, OFFSET_RASTER_POS_COLOR_R);
		cg_virtual_reg_t * regColorG0 = LOAD_DATA(block, regCommonFrom, OFFSET_RASTER_POS_COLOR_G);
		cg_virtual_reg_t * regColorB0 = LOAD_DATA(block, regCommonFrom, OFFSET_RASTER_POS_COLOR_B);
		cg_virtual_reg_t * regColorA0 = LOAD_DATA(block, regCommonFrom, OFFSET_RASTER_POS_COLOR_A);
		cg_virtual_reg_t * regInvZ0 = LOAD_DATA(block, regCommonFrom, OFFSET_RASTER_POS_WINDOW_INV_Z);
		cg_virtual_reg_t * regFog0 = LOAD_DATA(block, regCommonFrom, OFFSET_RASTER_POS_FOG);
		cg_virtual_reg_t * regDepth0 = LOAD_DATA(block, regCommonFrom, OFFSET_RASTER_POS_WINDOW_DEPTH);

		cg_virtual_reg_t * regTu0[EGL_NUM_TEXTURE_UNITS];
		cg_virtual_reg_t * regTv0[EGL_NUM_TEXTURE_UNITS];
		cg_virtual_reg_t * regTuOverZ0[EGL_NUM_TEXTURE_UNITS];
		cg_virtual_reg_t * regTvOverZ0[EGL_NUM_TEXTURE_UNITS];

		for (unit = 0; unit < EGL_NUM_TEXTURE_UNITS; ++unit) {
			regTu0[unit] = LOAD_DATA(block, regCommonFrom, OFFSET_RASTER_POS_TEX_TU + unit * sizeof(TexCoord));
			regTv0[unit] = LOAD_DATA(block, regCommonFrom, OFFSET_RASTER_POS_TEX_TV + unit * sizeof(TexCoord));

			ALLOC_REG(regTuOverZ0[unit]);
			ALLOC_REG(regTvOverZ0[unit]);

			FMUL		(regTuOverZ0[unit], regTu0[unit], regInvZ0);
			FMUL		(regTvOverZ0[unit], regTv0[unit], regInvZ0);
		}


			// -- end initialize

		// 	EGL_Fixed invSpan = EGL_Inverse(deltaX);
		// 	EGL_Fixed slope = EGL_Mul(EGL_Abs(deltaY), invSpan);
		// 	EGL_Fixed OneOverZTo = to.m_WindowCoords.invZ;
		DECL_REG	(regInvSpan);
		DECL_REG	(regSlope);

		FINV		(regInvSpan, regCommonDeltaX);
		FMUL		(regSlope, regAbsDeltaY, regInvSpan);

		cg_virtual_reg_t * regEndColorR0 = LOAD_DATA(block, regCommonTo, OFFSET_RASTER_POS_COLOR_R);
		cg_virtual_reg_t * regEndColorG0 = LOAD_DATA(block, regCommonTo, OFFSET_RASTER_POS_COLOR_G);
		cg_virtual_reg_t * regEndColorB0 = LOAD_DATA(block, regCommonTo, OFFSET_RASTER_POS_COLOR_B);
		cg_virtual_reg_t * regEndColorA0 = LOAD_DATA(block, regCommonTo, OFFSET_RASTER_POS_COLOR_A);
		cg_virtual_reg_t * regEndInvZ0 = LOAD_DATA(block, regCommonTo, OFFSET_RASTER_POS_WINDOW_INV_Z);
		cg_virtual_reg_t * regEndFog0 = LOAD_DATA(block, regCommonTo, OFFSET_RASTER_POS_FOG);
		cg_virtual_reg_t * regEndDepth0 = LOAD_DATA(block, regCommonTo, OFFSET_RASTER_POS_WINDOW_DEPTH);

		cg_virtual_reg_t * regEndTu0[EGL_NUM_TEXTURE_UNITS];
		cg_virtual_reg_t * regEndTv0[EGL_NUM_TEXTURE_UNITS];
		cg_virtual_reg_t * regEndTuOverZ0[EGL_NUM_TEXTURE_UNITS];
		cg_virtual_reg_t * regEndTvOverZ0[EGL_NUM_TEXTURE_UNITS];

		for (unit = 0; unit < EGL_NUM_TEXTURE_UNITS; ++unit) {
			regEndTu0[unit] = LOAD_DATA(block, regCommonTo, OFFSET_RASTER_POS_TEX_TU + unit * sizeof(TexCoord));
			regEndTv0[unit] = LOAD_DATA(block, regCommonTo, OFFSET_RASTER_POS_TEX_TV + unit * sizeof(TexCoord));

			ALLOC_REG(regEndTuOverZ0[unit]);
			ALLOC_REG(regEndTvOverZ0[unit]);

			FMUL		(regEndTuOverZ0[unit], regEndTu0[unit], regEndInvZ0);
			FMUL		(regEndTvOverZ0[unit], regEndTv0[unit], regEndInvZ0);
		}

		// -- increments(to, from, invSpan)

		// 	FractionalColor colorIncrement = (to.m_Color - from.m_Color) * invSpan;
		// 	EGL_Fixed deltaFog = EGL_Mul(to.m_FogDensity - from.m_FogDensity, invSpan);

		// 	EGL_Fixed deltaZ = EGL_Mul(OneOverZTo - OneOverZ, invSpan);

		// 	EGL_Fixed deltaU = EGL_Mul(EGL_Mul(to.m_TextureCoords.tu, OneOverZTo) -
		// 							   EGL_Mul(from.m_TextureCoords.tu, OneOverZ), invSpan);

		// 	EGL_Fixed deltaV = EGL_Mul(EGL_Mul(to.m_TextureCoords.tv, OneOverZTo) -
		// 							   EGL_Mul(from.m_TextureCoords.tv, OneOverZ), invSpan);

		// 	EGL_Fixed deltaDepth = EGL_Mul(to.m_WindowCoords.depth - from.m_WindowCoords.depth, invSpan);

		DECL_REG		(regDiffColorR);
		DECL_REG		(regDeltaColorR);
		DECL_REG		(regDiffColorG);
		DECL_REG		(regDeltaColorG);
		DECL_REG		(regDiffColorB);
		DECL_REG		(regDeltaColorB);
		DECL_REG		(regDiffColorA);
		DECL_REG		(regDeltaColorA);
		DECL_REG		(regDiffFog);
		DECL_REG		(regDeltaFog);
		DECL_REG		(regDiffDepth);
		DECL_REG		(regDeltaDepth);

		FSUB			(regDiffColorR, regEndColorR0, regColorR0);
		FMUL			(regDeltaColorR, regDiffColorR, regInvSpan);
		FSUB			(regDiffColorG, regEndColorG0, regColorG0);
		FMUL			(regDeltaColorG, regDiffColorG, regInvSpan);
		FSUB			(regDiffColorB, regEndColorB0, regColorB0);
		FMUL			(regDeltaColorB, regDiffColorB, regInvSpan);
		FSUB			(regDiffColorA, regEndColorA0, regColorA0);
		FMUL			(regDeltaColorA, regDiffColorA, regInvSpan);

		FSUB			(regDiffFog, regEndFog0, regFog0);
		FMUL			(regDeltaFog, regDiffFog, regInvSpan);
		FSUB			(regDiffDepth, regEndDepth0, regDepth0);
		FMUL			(regDeltaDepth, regDiffDepth, regInvSpan);

		DECL_REG		(regDiffInvZ);
		DECL_REG		(regDeltaInvZ);

		FSUB			(regDiffInvZ, regEndInvZ0, regInvZ0);
		FMUL			(regDeltaInvZ, regDiffInvZ, regInvSpan);

		cg_virtual_reg_t * regDiffTuOverZ[EGL_NUM_TEXTURE_UNITS];
		cg_virtual_reg_t * regDeltaTuOverZ[EGL_NUM_TEXTURE_UNITS];
		cg_virtual_reg_t * regDiffTvOverZ[EGL_NUM_TEXTURE_UNITS];
		cg_virtual_reg_t * regDeltaTvOverZ[EGL_NUM_TEXTURE_UNITS];

		for (unit = 0; unit < EGL_NUM_TEXTURE_UNITS; ++unit) {
			ALLOC_REG		(regDiffTuOverZ[unit]);
			ALLOC_REG		(regDeltaTuOverZ[unit]);
			ALLOC_REG		(regDiffTvOverZ[unit]);
			ALLOC_REG		(regDeltaTvOverZ[unit]);

			FSUB			(regDiffTuOverZ[unit], regEndTuOverZ0[unit], regTuOverZ0[unit]);
			FMUL			(regDeltaTuOverZ[unit], regDiffTuOverZ[unit], regInvSpan);
			FSUB			(regDiffTvOverZ[unit], regEndTvOverZ0[unit], regTvOverZ0[unit]);
			FMUL			(regDeltaTvOverZ[unit], regDiffTvOverZ[unit], regInvSpan);
		}
		// -- end increments

		// 	I32 y = EGL_IntFromFixed(from.m_WindowCoords.y + ((EGL_ONE)/2-1));
		cg_virtual_reg_t * regFromY0 = LOAD_DATA(block, regCommonFrom, OFFSET_RASTER_POS_WINDOW_Y);

		DECL_CONST_REG	(regHalf2, 0x7fff);
		DECL_REG		(regFromY0PlusHalf);
		DECL_REG		(regY0);

		FADD			(regFromY0PlusHalf, regFromY0, regHalf2);
		TRUNC			(regY0, regFromY0PlusHalf);

		// 	EGL_Fixed error = 0;
		DECL_CONST_REG	(regError0, 0);

		block = cg_block_create(procedure, 4);
		cg_block_ref_t * blockRefBeginLoop = cg_block_ref_create(procedure);
		blockRefBeginLoop->block = block;

		// 	for (; x < endX; ++x) {

		// --- variables for loop entry
		DECL_REG		(regLoopX);
		DECL_REG		(regLoopY);
		DECL_REG		(regLoopError);

		DECL_REG		(regLoopOneOverZ);

		DECL_REG		(regLoopColorR);
		DECL_REG		(regLoopColorG);
		DECL_REG		(regLoopColorB);
		DECL_REG		(regLoopColorA);

		DECL_REG		(regLoopDepth);
		DECL_REG		(regLoopFog);

		// --- variables for loop exit
		DECL_REG		(regEndLoopX);
		DECL_REG		(regEndLoopY);
		DECL_REG		(regEndLoopError);

		DECL_REG		(regEndLoopOneOverZ);

		DECL_REG		(regEndLoopColorR);
		DECL_REG		(regEndLoopColorG);
		DECL_REG		(regEndLoopColorB);
		DECL_REG		(regEndLoopColorA);

		DECL_REG		(regEndLoopDepth);
		DECL_REG		(regEndLoopFog);

		PHI				(regLoopX, cg_create_virtual_reg_list(procedure->module->heap, regEndLoopX, regCommonX, NULL));
		PHI				(regLoopY, cg_create_virtual_reg_list(procedure->module->heap, regEndLoopY, regY0, NULL));
		PHI				(regLoopError, cg_create_virtual_reg_list(procedure->module->heap, regEndLoopError, regError0, NULL));

		PHI				(regLoopOneOverZ, cg_create_virtual_reg_list(procedure->module->heap, regEndLoopOneOverZ, regInvZ0, NULL));

		PHI				(regLoopColorR, cg_create_virtual_reg_list(procedure->module->heap, regEndLoopColorR, regColorR0, NULL));
		PHI				(regLoopColorG, cg_create_virtual_reg_list(procedure->module->heap, regEndLoopColorG, regColorG0, NULL));
		PHI				(regLoopColorB, cg_create_virtual_reg_list(procedure->module->heap, regEndLoopColorB, regColorB0, NULL));
		PHI				(regLoopColorA, cg_create_virtual_reg_list(procedure->module->heap, regEndLoopColorA, regColorA0, NULL));

		PHI				(regLoopDepth, cg_create_virtual_reg_list(procedure->module->heap, regEndLoopDepth, regDepth0, NULL));
		PHI				(regLoopFog, cg_create_virtual_reg_list(procedure->module->heap, regEndLoopFog, regFog0, NULL));

		cg_virtual_reg_t * regLoopTuOverZ[EGL_NUM_TEXTURE_UNITS];
		cg_virtual_reg_t * regLoopTvOverZ[EGL_NUM_TEXTURE_UNITS];
		cg_virtual_reg_t * regEndLoopTuOverZ[EGL_NUM_TEXTURE_UNITS];
		cg_virtual_reg_t * regEndLoopTvOverZ[EGL_NUM_TEXTURE_UNITS];

		for (unit = 0; unit < EGL_NUM_TEXTURE_UNITS; ++unit) {
			ALLOC_REG		(regLoopTuOverZ[unit]);
			ALLOC_REG		(regLoopTvOverZ[unit]);
			ALLOC_REG		(regEndLoopTuOverZ[unit]);
			ALLOC_REG		(regEndLoopTvOverZ[unit]);

			PHI				(regLoopTuOverZ[unit], cg_create_virtual_reg_list(procedure->module->heap, regEndLoopTuOverZ[unit], regTuOverZ0[unit], NULL));
			PHI				(regLoopTvOverZ[unit], cg_create_virtual_reg_list(procedure->module->heap, regEndLoopTvOverZ[unit], regTvOverZ0[unit], NULL));
		}
		
		// 		EGL_Fixed z = EGL_Inverse(OneOverZ);
		// 		OneOverZ += deltaZ;

		DECL_REG		(regLoopZ);
		FINV			(regLoopZ, regLoopOneOverZ);
		FADD			(regEndLoopOneOverZ, regLoopOneOverZ, regDeltaInvZ);

		cg_virtual_reg_t * regLoopTu[EGL_NUM_TEXTURE_UNITS];
		cg_virtual_reg_t * regLoopTv[EGL_NUM_TEXTURE_UNITS];

		for (unit = 0; unit < EGL_NUM_TEXTURE_UNITS; ++unit) {
			ALLOC_REG		(regLoopTu[unit]);
			ALLOC_REG		(regLoopTv[unit]);

			// 		EGL_Fixed tu = EGL_Mul(tuOverZ, z);
			// 		EGL_Fixed tv = EGL_Mul(tvOverZ, z);
			// 		tuOverZ += deltaU;
			// 		tvOverZ += deltaV;
			FMUL			(regLoopTu[unit], regLoopTuOverZ[unit], regLoopZ);
			FADD			(regEndLoopTuOverZ[unit], regLoopTuOverZ[unit], regDeltaTuOverZ[unit]);
			FMUL			(regLoopTv[unit], regLoopTvOverZ[unit], regLoopZ);
			FADD			(regEndLoopTvOverZ[unit], regLoopTvOverZ[unit], regDeltaTvOverZ[unit]);

			info.regU[unit] = regLoopTu[unit];
			info.regV[unit] = regLoopTv[unit]; 
		}


		// 		Fragment(x, y, depth, tu, tv, fogDensity, baseColor);
		info.regX = regLoopX;
		info.regY = regLoopY;

		info.regFog = regLoopFog;
		info.regDepth = regLoopDepth;
		info.regR = regLoopColorR;
		info.regG = regLoopColorG;
		info.regB = regLoopColorB;
		info.regA = regLoopColorA;

		cg_block_ref_t * postFragment = cg_block_ref_create(procedure);

		GenerateFragment(procedure, block, postFragment, info, 5, true);

		block = cg_block_create(procedure, 5);
		postFragment->block = block;

		// 		error += slope;

		DECL_REG		(regIncError);
		FADD			(regIncError, regLoopError, regSlope);

		// 		if (error > EGL_ONE) {
		DECL_CONST_REG	(regLoopOne, EGL_ONE);
		DECL_FLAGS		(regErrorOverflow);
		FCMP			(regErrorOverflow, regIncError, regLoopOne);
		cg_block_ref_t * noOverflow = cg_block_ref_create(procedure);

		BLE				(regErrorOverflow, noOverflow);

		block = cg_block_create(procedure, 5);

		// 			y += yIncrement;
		// 			error -= EGL_ONE;
		DECL_REG		(regIncY);
		DECL_REG		(regCorrectedError);

		FADD			(regIncY, regLoopY, regYIncrement);
		FSUB			(regCorrectedError, regIncError, regLoopOne);
		// 		}

		block = cg_block_create(procedure, 5);
		noOverflow->block = block;

		PHI				(regEndLoopY, cg_create_virtual_reg_list(procedure->module->heap, regLoopY, regIncY, NULL));
		PHI				(regEndLoopError, cg_create_virtual_reg_list(procedure->module->heap, regIncError, regCorrectedError, NULL));

		// 		baseColor += colorIncrement;
		FADD			(regEndLoopColorR,	regLoopColorR, regDeltaColorR);
		FADD			(regEndLoopColorG,	regLoopColorG, regDeltaColorG);
		FADD			(regEndLoopColorB,	regLoopColorB, regDeltaColorB);
		FADD			(regEndLoopColorA,	regLoopColorA, regDeltaColorA);

		// 		depth += deltaDepth;
		// 		fogDensity += deltaFog;
		FADD			(regEndLoopDepth,	regLoopDepth, regDeltaDepth);
		FADD			(regEndLoopFog,		regLoopFog, regDeltaFog);

		DECL_CONST_REG	(regConstInt1, 1);
		DECL_FLAGS		(regCompareX);
		FADD			(regEndLoopX, regLoopX, regConstInt1);
		FCMP			(regCompareX, regEndLoopX, regCommonEndX);
		BLT				(regCompareX, blockRefBeginLoop);

		// 	}
	}

	BRA		(blockRefEndProc);

	// } else {
		// Bresenham along y-axis

	block = cg_block_create(procedure, 1);
	blockRefRasterY->block = block;

	// 	const RasterPos *start, *end;

	// 	I32 y;
	// 	I32 endY;
	// 	EGL_Fixed roundedY; //, preStepY;

	// 	if (deltaY < 0) {
	// 		deltaY = -deltaY;
	// 		deltaX = -deltaX;
	// 		start = &p_to;
	// 		end = &p_from;
	// 		roundedY = EGL_NearestInt(p_to.m_WindowCoords.y + 1);
	// 		y = EGL_IntFromFixed(roundedY);
	// 		endY = EGL_IntFromFixed(p_from.m_WindowCoords.y + ((EGL_ONE)/2));
	// 	} else {
	// 		start = &p_from;
	// 		end = &p_to;
	// 		roundedY = EGL_NearestInt(p_from.m_WindowCoords.y);
	// 		y = EGL_IntFromFixed(roundedY);
	// 		endY = EGL_IntFromFixed(p_to.m_WindowCoords.y + ((EGL_ONE)/2-1));
	// 	}

	// 	const RasterPos& from = *start;
	// 	const RasterPos& to = *end;

		//-- initialize with from vertex attributes
	// 	FractionalColor baseColor = from.m_Color;
	// 	EGL_Fixed OneOverZ = from.m_WindowCoords.invZ;
	// 	EGL_Fixed tuOverZ = EGL_Mul(from.m_TextureCoords.tu, OneOverZ);
	// 	EGL_Fixed tvOverZ = EGL_Mul(from.m_TextureCoords.tv, OneOverZ);
	// 	EGL_Fixed fogDensity = from.m_FogDensity;
	// 	EGL_Fixed depth = from.m_WindowCoords.depth;
		//-- end initialize

	// 	EGL_Fixed invSpan = EGL_Inverse(deltaY);
	// 	EGL_Fixed slope = EGL_Mul(EGL_Abs(deltaX), invSpan);
	// 	EGL_Fixed OneOverZTo = to.m_WindowCoords.invZ;

		// -- increments(to, from, invSpan)
	// 	FractionalColor colorIncrement = (to.m_Color - from.m_Color) * invSpan;
	// 	EGL_Fixed deltaFog = EGL_Mul(to.m_FogDensity - from.m_FogDensity, invSpan);

	// 	EGL_Fixed deltaZ = EGL_Mul(OneOverZTo - OneOverZ, invSpan);

	// 	EGL_Fixed deltaU = EGL_Mul(EGL_Mul(to.m_TextureCoords.tu, OneOverZTo) -
	// 							   EGL_Mul(from.m_TextureCoords.tu, OneOverZ), invSpan);

	// 	EGL_Fixed deltaV = EGL_Mul(EGL_Mul(to.m_TextureCoords.tv, OneOverZTo) -
	// 							   EGL_Mul(from.m_TextureCoords.tv, OneOverZ), invSpan);

	// 	EGL_Fixed deltaDepth = EGL_Mul(to.m_WindowCoords.depth - from.m_WindowCoords.depth, invSpan);
		// -- end increments

	// 	I32 x = EGL_IntFromFixed(from.m_WindowCoords.x + ((EGL_ONE)/2-1));

	// 	I32 xIncrement = (deltaX > 0) ? 1 : -1;
	// 	EGL_Fixed error = 0;

		// can have xIncrement; yIncrement; xBaseIncrement, yBaseIncrement
		// then both x/y loops become the same
		// question: how to add correct mipmap selection?

	// 	for (; y < endY; ++y) {

	// 		EGL_Fixed z = EGL_Inverse(OneOverZ);
	// 		EGL_Fixed tu = EGL_Mul(tuOverZ, z);
	// 		EGL_Fixed tv = EGL_Mul(tvOverZ, z);

	// 		Fragment(x, y, depth, tu, tv, fogDensity, baseColor);

	// 		error += slope;
	// 		if (error > EGL_ONE) {
	// 			x += xIncrement;
	// 			error -= EGL_ONE;
	// 		}

	// 		baseColor += colorIncrement;
	// 		depth += deltaDepth;
	// 		OneOverZ += deltaZ;
	// 		tuOverZ += deltaU;
	// 		tvOverZ += deltaV;
	// 		fogDensity += deltaFog;
	// 	}
	// }

	{
		// 	const RasterPos *start, *end;

		// 	I32 x;
		// 	I32 endX;
		// 	EGL_Fixed roundedX;

		// 	if (deltaX < 0) {
		DECL_FLAGS		(regSignY);
		DECL_CONST_REG	(regZero, 0);
		
		FCMP		(regSignY, regDeltaY, regZero);

		cg_block_ref_t * blockRefPositiveDeltaY = cg_block_ref_create(procedure);

		BGE			(regSignY, blockRefPositiveDeltaY);

		block = cg_block_create(procedure, 1);

		// 		deltaY = -deltaY;
		// 		deltaX = -deltaX;
		DECL_REG	(regMinusDeltaX);
		DECL_REG	(regMinusDeltaY);

		FNEG		(regMinusDeltaX, regDeltaX);
		FNEG		(regMinusDeltaY, regDeltaY);

		// 		start = &p_to;
		// 		end = &p_from;
		DECL_REG	(regStart0);
		DECL_REG	(regEnd0);

		OR			(regStart0, regTo, regTo);
		OR			(regEnd0, regFrom, regFrom);

		// 		roundedX = EGL_NearestInt(p_to.m_WindowCoords.x + 1);
		// 		x = EGL_IntFromFixed(roundedX);
		// 		endX = EGL_IntFromFixed(p_from.m_WindowCoords.x + ((EGL_ONE)/2));
		DECL_REG	(regY0);
		DECL_REG	(regEndY0);
		DECL_CONST_REG	(regHalf, 0x8000);
		DECL_REG	(regYToPlusHalf0);
		DECL_REG	(regYFromPlusHalf0);

		FADD		(regYToPlusHalf0, regToY, regHalf);
		TRUNC		(regY0, regYToPlusHalf0);
		FADD		(regYFromPlusHalf0, regFromY, regHalf);
		TRUNC		(regEndY0, regYFromPlusHalf0);

		cg_block_ref_t * blockRefPostDeltaY = cg_block_ref_create(procedure);

		BRA			(blockRefPostDeltaY);

		// 	} else {
		block = cg_block_create(procedure, 1);
		blockRefPositiveDeltaY->block = block;

		// 		start = &p_from;
		// 		end = &p_to;
		DECL_REG	(regStart1);
		DECL_REG	(regEnd1);

		OR			(regStart1, regFrom, regFrom);
		OR			(regEnd1, regTo, regTo);

		// 		roundedX = EGL_NearestInt(p_from.m_WindowCoords.x);
		// 		x = EGL_IntFromFixed(roundedX);
		// 		endX = EGL_IntFromFixed(p_to.m_WindowCoords.x + ((EGL_ONE)/2-1));
		DECL_REG	(regY1);
		DECL_REG	(regEndY1);
		DECL_CONST_REG	(regHalf1, 0x7fff);
		DECL_REG	(regYToPlusHalf1);
		DECL_REG	(regYFromPlusHalf1);

		FADD		(regYToPlusHalf1, regToY, regHalf1);
		TRUNC		(regEndY1, regYToPlusHalf1);
		FADD		(regYFromPlusHalf1, regFromY, regHalf1);
		TRUNC		(regY1, regYFromPlusHalf1);

		// 	}

		block = cg_block_create(procedure, 1);
		blockRefPostDeltaY->block = block;

		DECL_REG	(regCommonDeltaX);
		DECL_REG	(regCommonDeltaY);

		PHI			(regCommonDeltaX, cg_create_virtual_reg_list(procedure->module->heap, regDeltaX, regMinusDeltaX, NULL));
		PHI			(regCommonDeltaY, cg_create_virtual_reg_list(procedure->module->heap, regDeltaY, regMinusDeltaY, NULL));

		DECL_REG	(regCommonY);
		DECL_REG	(regCommonEndY);

		PHI			(regCommonY, cg_create_virtual_reg_list(procedure->module->heap, regY0, regY1, NULL));
		PHI			(regCommonEndY, cg_create_virtual_reg_list(procedure->module->heap, regEndY0, regEndY1, NULL));

		// 	const RasterPos& from = *start;
		// 	const RasterPos& to = *end;

		DECL_REG	(regCommonFrom);
		DECL_REG	(regCommonTo);

		PHI			(regCommonFrom, cg_create_virtual_reg_list(procedure->module->heap, regStart0, regStart1, NULL));
		PHI			(regCommonTo, cg_create_virtual_reg_list(procedure->module->heap, regEnd0, regEnd1, NULL));

		// 	I32 yIncrement = (deltaY > 0) ? 1 : -1;
		DECL_FLAGS		(regSignX);
		
		FCMP		(regSignX, regCommonDeltaX, regZero);

		cg_block_ref_t * blockRefPositiveDeltaX = cg_block_ref_create(procedure);

		BGT			(regSignX, blockRefPositiveDeltaX);

		block = cg_block_create(procedure, 1);

		DECL_CONST_REG	(regXIncrementNeg, -1);

		cg_block_ref_t * blockRefCommonDeltaX = cg_block_ref_create(procedure);

		BRA			(blockRefCommonDeltaX);

		block = cg_block_create(procedure, 1);
		blockRefPositiveDeltaX->block = block;

		DECL_CONST_REG	(regXIncrementPos, 1);

		block = cg_block_create(procedure, 1);
		blockRefCommonDeltaX->block = block;

		DECL_REG		(regXIncrement);
		PHI				(regXIncrement, cg_create_virtual_reg_list(procedure->module->heap, regXIncrementPos, regXIncrementNeg, NULL));

			// -- initialize with starting vertex attributes
		// 	FractionalColor baseColor = from.m_Color;
		// 	EGL_Fixed OneOverZ = from.m_WindowCoords.invZ;
		// 	EGL_Fixed tuOverZ = EGL_Mul(from.m_TextureCoords.tu, OneOverZ);
		// 	EGL_Fixed tvOverZ = EGL_Mul(from.m_TextureCoords.tv, OneOverZ);
		// 	EGL_Fixed fogDensity = from.m_FogDensity;
		// 	EGL_Fixed depth = from.m_WindowCoords.depth;

		cg_virtual_reg_t * regColorR0 = LOAD_DATA(block, regCommonFrom, OFFSET_RASTER_POS_COLOR_R);
		cg_virtual_reg_t * regColorG0 = LOAD_DATA(block, regCommonFrom, OFFSET_RASTER_POS_COLOR_G);
		cg_virtual_reg_t * regColorB0 = LOAD_DATA(block, regCommonFrom, OFFSET_RASTER_POS_COLOR_B);
		cg_virtual_reg_t * regColorA0 = LOAD_DATA(block, regCommonFrom, OFFSET_RASTER_POS_COLOR_A);
		cg_virtual_reg_t * regInvZ0 = LOAD_DATA(block, regCommonFrom, OFFSET_RASTER_POS_WINDOW_INV_Z);
		cg_virtual_reg_t * regFog0 = LOAD_DATA(block, regCommonFrom, OFFSET_RASTER_POS_FOG);
		cg_virtual_reg_t * regDepth0 = LOAD_DATA(block, regCommonFrom, OFFSET_RASTER_POS_WINDOW_DEPTH);

		cg_virtual_reg_t * regTu0[EGL_NUM_TEXTURE_UNITS];
		cg_virtual_reg_t * regTv0[EGL_NUM_TEXTURE_UNITS];
		cg_virtual_reg_t * regTuOverZ0[EGL_NUM_TEXTURE_UNITS];
		cg_virtual_reg_t * regTvOverZ0[EGL_NUM_TEXTURE_UNITS];

		for (unit = 0; unit < EGL_NUM_TEXTURE_UNITS; ++unit) {
			regTu0[unit] = LOAD_DATA(block, regCommonFrom, OFFSET_RASTER_POS_TEX_TU + unit * sizeof(TexCoord));
			regTv0[unit] = LOAD_DATA(block, regCommonFrom, OFFSET_RASTER_POS_TEX_TV + unit * sizeof(TexCoord));

			ALLOC_REG	(regTuOverZ0[unit]);
			ALLOC_REG	(regTvOverZ0[unit]);

			FMUL		(regTuOverZ0[unit], regTu0[unit], regInvZ0);
			FMUL		(regTvOverZ0[unit], regTv0[unit], regInvZ0);
		}

			// -- end initialize

		// 	EGL_Fixed invSpan = EGL_Inverse(deltaX);
		// 	EGL_Fixed slope = EGL_Mul(EGL_Abs(deltaY), invSpan);
		// 	EGL_Fixed OneOverZTo = to.m_WindowCoords.invZ;
		DECL_REG	(regInvSpan);
		DECL_REG	(regSlope);

		FINV		(regInvSpan, regCommonDeltaY);
		FMUL		(regSlope, regAbsDeltaX, regInvSpan);

		cg_virtual_reg_t * regEndColorR0 = LOAD_DATA(block, regCommonTo, OFFSET_RASTER_POS_COLOR_R);
		cg_virtual_reg_t * regEndColorG0 = LOAD_DATA(block, regCommonTo, OFFSET_RASTER_POS_COLOR_G);
		cg_virtual_reg_t * regEndColorB0 = LOAD_DATA(block, regCommonTo, OFFSET_RASTER_POS_COLOR_B);
		cg_virtual_reg_t * regEndColorA0 = LOAD_DATA(block, regCommonTo, OFFSET_RASTER_POS_COLOR_A);
		cg_virtual_reg_t * regEndFog0 = LOAD_DATA(block, regCommonTo, OFFSET_RASTER_POS_FOG);
		cg_virtual_reg_t * regEndDepth0 = LOAD_DATA(block, regCommonTo, OFFSET_RASTER_POS_WINDOW_DEPTH);
		cg_virtual_reg_t * regEndInvZ0 = LOAD_DATA(block, regCommonTo, OFFSET_RASTER_POS_WINDOW_INV_Z);

		cg_virtual_reg_t * regEndTu0[EGL_NUM_TEXTURE_UNITS];
		cg_virtual_reg_t * regEndTv0[EGL_NUM_TEXTURE_UNITS];
		cg_virtual_reg_t * regEndTuOverZ0[EGL_NUM_TEXTURE_UNITS];
		cg_virtual_reg_t * regEndTvOverZ0[EGL_NUM_TEXTURE_UNITS];

		for (unit = 0; unit < EGL_NUM_TEXTURE_UNITS; ++unit) {
			regEndTu0[unit] = LOAD_DATA(block, regCommonTo, OFFSET_RASTER_POS_TEX_TU + unit * sizeof(TexCoord));
			regEndTv0[unit] = LOAD_DATA(block, regCommonTo, OFFSET_RASTER_POS_TEX_TV + unit * sizeof(TexCoord));

			ALLOC_REG	(regEndTuOverZ0[unit]);
			ALLOC_REG	(regEndTvOverZ0[unit]);

			FMUL		(regEndTuOverZ0[unit], regEndTu0[unit], regEndInvZ0);
			FMUL		(regEndTvOverZ0[unit], regEndTv0[unit], regEndInvZ0);
		}


		// -- increments(to, from, invSpan)

		// 	FractionalColor colorIncrement = (to.m_Color - from.m_Color) * invSpan;
		// 	EGL_Fixed deltaFog = EGL_Mul(to.m_FogDensity - from.m_FogDensity, invSpan);

		// 	EGL_Fixed deltaZ = EGL_Mul(OneOverZTo - OneOverZ, invSpan);

		// 	EGL_Fixed deltaU = EGL_Mul(EGL_Mul(to.m_TextureCoords.tu, OneOverZTo) -
		// 							   EGL_Mul(from.m_TextureCoords.tu, OneOverZ), invSpan);

		// 	EGL_Fixed deltaV = EGL_Mul(EGL_Mul(to.m_TextureCoords.tv, OneOverZTo) -
		// 							   EGL_Mul(from.m_TextureCoords.tv, OneOverZ), invSpan);

		// 	EGL_Fixed deltaDepth = EGL_Mul(to.m_WindowCoords.depth - from.m_WindowCoords.depth, invSpan);

		DECL_REG		(regDiffColorR);
		DECL_REG		(regDeltaColorR);
		DECL_REG		(regDiffColorG);
		DECL_REG		(regDeltaColorG);
		DECL_REG		(regDiffColorB);
		DECL_REG		(regDeltaColorB);
		DECL_REG		(regDiffColorA);
		DECL_REG		(regDeltaColorA);
		DECL_REG		(regDiffFog);
		DECL_REG		(regDeltaFog);
		DECL_REG		(regDiffDepth);
		DECL_REG		(regDeltaDepth);

		FSUB			(regDiffColorR, regEndColorR0, regColorR0);
		FMUL			(regDeltaColorR, regDiffColorR, regInvSpan);
		FSUB			(regDiffColorG, regEndColorG0, regColorG0);
		FMUL			(regDeltaColorG, regDiffColorG, regInvSpan);
		FSUB			(regDiffColorB, regEndColorB0, regColorB0);
		FMUL			(regDeltaColorB, regDiffColorB, regInvSpan);
		FSUB			(regDiffColorA, regEndColorA0, regColorA0);
		FMUL			(regDeltaColorA, regDiffColorA, regInvSpan);

		FSUB			(regDiffFog, regEndFog0, regFog0);
		FMUL			(regDeltaFog, regDiffFog, regInvSpan);
		FSUB			(regDiffDepth, regEndDepth0, regDepth0);
		FMUL			(regDeltaDepth, regDiffDepth, regInvSpan);

		DECL_REG		(regDiffInvZ);
		DECL_REG		(regDeltaInvZ);

		FSUB			(regDiffInvZ, regEndInvZ0, regInvZ0);
		FMUL			(regDeltaInvZ, regDiffInvZ, regInvSpan);

		cg_virtual_reg_t * regDiffTuOverZ[EGL_NUM_TEXTURE_UNITS];
		cg_virtual_reg_t * regDeltaTuOverZ[EGL_NUM_TEXTURE_UNITS];
		cg_virtual_reg_t * regDiffTvOverZ[EGL_NUM_TEXTURE_UNITS];
		cg_virtual_reg_t * regDeltaTvOverZ[EGL_NUM_TEXTURE_UNITS];

		for (unit = 0; unit < EGL_NUM_TEXTURE_UNITS; ++unit) {
			ALLOC_REG		(regDiffTuOverZ[unit]);
			ALLOC_REG		(regDeltaTuOverZ[unit]);
			ALLOC_REG		(regDiffTvOverZ[unit]);
			ALLOC_REG		(regDeltaTvOverZ[unit]);

			FSUB			(regDiffTuOverZ[unit], regEndTuOverZ0[unit], regTuOverZ0[unit]);
			FMUL			(regDeltaTuOverZ[unit], regDiffTuOverZ[unit], regInvSpan);
			FSUB			(regDiffTvOverZ[unit], regEndTvOverZ0[unit], regTvOverZ0[unit]);
			FMUL			(regDeltaTvOverZ[unit], regDiffTvOverZ[unit], regInvSpan);
		}
		// -- end increments

		// 	I32 y = EGL_IntFromFixed(from.m_WindowCoords.y + ((EGL_ONE)/2-1));
		cg_virtual_reg_t * regFromX0 = LOAD_DATA(block, regCommonFrom, OFFSET_RASTER_POS_WINDOW_X);

		DECL_CONST_REG	(regHalf2, 0x7fff);
		DECL_REG		(regFromX0PlusHalf);
		DECL_REG		(regX0);

		FADD			(regFromX0PlusHalf, regFromX0, regHalf2);
		TRUNC			(regX0, regFromX0PlusHalf);

		// 	EGL_Fixed error = 0;
		DECL_CONST_REG	(regError0, 0);

		block = cg_block_create(procedure, 4);
		cg_block_ref_t * blockRefBeginLoop = cg_block_ref_create(procedure);
		blockRefBeginLoop->block = block;

		// 	for (; x < endX; ++x) {

		// --- variables for loop entry
		DECL_REG		(regLoopX);
		DECL_REG		(regLoopY);
		DECL_REG		(regLoopError);

		DECL_REG		(regLoopOneOverZ);

		DECL_REG		(regLoopColorR);
		DECL_REG		(regLoopColorG);
		DECL_REG		(regLoopColorB);
		DECL_REG		(regLoopColorA);

		DECL_REG		(regLoopDepth);
		DECL_REG		(regLoopFog);

		// --- variables for loop exit
		DECL_REG		(regEndLoopX);
		DECL_REG		(regEndLoopY);
		DECL_REG		(regEndLoopError);

		DECL_REG		(regEndLoopOneOverZ);

		DECL_REG		(regEndLoopColorR);
		DECL_REG		(regEndLoopColorG);
		DECL_REG		(regEndLoopColorB);
		DECL_REG		(regEndLoopColorA);

		DECL_REG		(regEndLoopDepth);
		DECL_REG		(regEndLoopFog);

		PHI				(regLoopX, cg_create_virtual_reg_list(procedure->module->heap, regEndLoopX, regX0, NULL));
		PHI				(regLoopY, cg_create_virtual_reg_list(procedure->module->heap, regEndLoopY, regCommonY, NULL));
		PHI				(regLoopError, cg_create_virtual_reg_list(procedure->module->heap, regEndLoopError, regError0, NULL));

		PHI				(regLoopOneOverZ, cg_create_virtual_reg_list(procedure->module->heap, regEndLoopOneOverZ, regInvZ0, NULL));

		PHI				(regLoopColorR, cg_create_virtual_reg_list(procedure->module->heap, regEndLoopColorR, regColorR0, NULL));
		PHI				(regLoopColorG, cg_create_virtual_reg_list(procedure->module->heap, regEndLoopColorG, regColorG0, NULL));
		PHI				(regLoopColorB, cg_create_virtual_reg_list(procedure->module->heap, regEndLoopColorB, regColorB0, NULL));
		PHI				(regLoopColorA, cg_create_virtual_reg_list(procedure->module->heap, regEndLoopColorA, regColorA0, NULL));

		PHI				(regLoopDepth, cg_create_virtual_reg_list(procedure->module->heap, regEndLoopDepth, regDepth0, NULL));
		PHI				(regLoopFog, cg_create_virtual_reg_list(procedure->module->heap, regEndLoopFog, regFog0, NULL));
		
		cg_virtual_reg_t * regLoopTuOverZ[EGL_NUM_TEXTURE_UNITS];
		cg_virtual_reg_t * regLoopTvOverZ[EGL_NUM_TEXTURE_UNITS];
		cg_virtual_reg_t * regEndLoopTuOverZ[EGL_NUM_TEXTURE_UNITS];
		cg_virtual_reg_t * regEndLoopTvOverZ[EGL_NUM_TEXTURE_UNITS];

		for (unit = 0; unit < EGL_NUM_TEXTURE_UNITS; ++unit) {
			ALLOC_REG		(regLoopTuOverZ[unit]);
			ALLOC_REG		(regLoopTvOverZ[unit]);
			ALLOC_REG		(regEndLoopTuOverZ[unit]);
			ALLOC_REG		(regEndLoopTvOverZ[unit]);

			PHI				(regLoopTuOverZ[unit], cg_create_virtual_reg_list(procedure->module->heap, regEndLoopTuOverZ[unit], regTuOverZ0[unit], NULL));
			PHI				(regLoopTvOverZ[unit], cg_create_virtual_reg_list(procedure->module->heap, regEndLoopTvOverZ[unit], regTvOverZ0[unit], NULL));
		}
		
		// 		EGL_Fixed z = EGL_Inverse(OneOverZ);
		// 		OneOverZ += deltaZ;

		DECL_REG		(regLoopZ);
		FINV			(regLoopZ, regLoopOneOverZ);
		FADD			(regEndLoopOneOverZ, regLoopOneOverZ, regDeltaInvZ);

		cg_virtual_reg_t * regLoopTu[EGL_NUM_TEXTURE_UNITS];
		cg_virtual_reg_t * regLoopTv[EGL_NUM_TEXTURE_UNITS];

		for (unit = 0; unit < EGL_NUM_TEXTURE_UNITS; ++unit) {
			ALLOC_REG		(regLoopTu[unit]);
			ALLOC_REG		(regLoopTv[unit]);

			// 		EGL_Fixed tu = EGL_Mul(tuOverZ, z);
			// 		EGL_Fixed tv = EGL_Mul(tvOverZ, z);
			// 		tuOverZ += deltaU;
			// 		tvOverZ += deltaV;
			FMUL			(regLoopTu[unit], regLoopTuOverZ[unit], regLoopZ);
			FADD			(regEndLoopTuOverZ[unit], regLoopTuOverZ[unit], regDeltaTuOverZ[unit]);
			FMUL			(regLoopTv[unit], regLoopTvOverZ[unit], regLoopZ);
			FADD			(regEndLoopTvOverZ[unit], regLoopTvOverZ[unit], regDeltaTvOverZ[unit]);

			info.regU[unit] = regLoopTu[unit];
			info.regV[unit] = regLoopTv[unit]; 
		}

		// 		Fragment(x, y, depth, tu, tv, fogDensity, baseColor);
		info.regX = regLoopX;
		info.regY = regLoopY;

		info.regFog = regLoopFog;
		info.regDepth = regLoopDepth;
		info.regR = regLoopColorR;
		info.regG = regLoopColorG;
		info.regB = regLoopColorB;
		info.regA = regLoopColorA;

		cg_block_ref_t * postFragment = cg_block_ref_create(procedure);

		GenerateFragment(procedure, block, postFragment, info, 5, true);

		block = cg_block_create(procedure, 5);
		postFragment->block = block;

		// 		error += slope;

		DECL_REG		(regIncError);
		FADD			(regIncError, regLoopError, regSlope);

		// 		if (error > EGL_ONE) {
		DECL_CONST_REG	(regLoopOne, EGL_ONE);
		DECL_FLAGS		(regErrorOverflow);
		FCMP			(regErrorOverflow, regIncError, regLoopOne);
		cg_block_ref_t * noOverflow = cg_block_ref_create(procedure);

		BLE				(regErrorOverflow, noOverflow);

		block = cg_block_create(procedure, 5);

		// 			y += yIncrement;
		// 			error -= EGL_ONE;
		DECL_REG		(regIncX);
		DECL_REG		(regCorrectedError);

		FADD			(regIncX, regLoopX, regXIncrement);
		FSUB			(regCorrectedError, regIncError, regLoopOne);
		// 		}

		block = cg_block_create(procedure, 5);
		noOverflow->block = block;

		PHI				(regEndLoopX, cg_create_virtual_reg_list(procedure->module->heap, regLoopX, regIncX, NULL));
		PHI				(regEndLoopError, cg_create_virtual_reg_list(procedure->module->heap, regIncError, regCorrectedError, NULL));

		// 		baseColor += colorIncrement;
		FADD			(regEndLoopColorR,	regLoopColorR, regDeltaColorR);
		FADD			(regEndLoopColorG,	regLoopColorG, regDeltaColorG);
		FADD			(regEndLoopColorB,	regLoopColorB, regDeltaColorB);
		FADD			(regEndLoopColorA,	regLoopColorA, regDeltaColorA);

		// 		depth += deltaDepth;
		// 		fogDensity += deltaFog;
		FADD			(regEndLoopDepth,	regLoopDepth, regDeltaDepth);
		FADD			(regEndLoopFog,		regLoopFog, regDeltaFog);

		DECL_CONST_REG	(regConstInt1, 1);
		DECL_FLAGS		(regCompareY);
		FADD			(regEndLoopY, regLoopY, regConstInt1);
		FCMP			(regCompareY, regEndLoopY, regCommonEndY);
		BLT				(regCompareY, blockRefBeginLoop);

		// 	}
	}

	block = cg_block_create(procedure, 1);
	blockRefEndProc->block = block;

	RET();
}

