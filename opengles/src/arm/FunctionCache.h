#ifndef EGL_FUNCTION_CACHE_H
#define EGL_FUNCTION_CACHE_H 1

// ==========================================================================
//
// FunctionCache.h		Cache of compiled functions for 3D Rendering Library
//
// --------------------------------------------------------------------------
//
// 03-08-2004	Hans-Martin Will	initial version
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


#include "OGLES.h"
#include "RasterizerState.h"
#include "Rasterizer.h"


namespace EGL {

	struct FunctionInfo;
	class CodeGenerator;

	class OGLES_API FunctionCache {
#if defined(EGL_ON_LINUX)
		friend class CodeGenerator;
#else
		friend CodeGenerator;
#endif
	public:
		enum FunctionType {
			FunctionTypeScanline,
			FunctionTypePoint,
			FunctionTypeLine,
			FunctionTypeTriangle
		};

	public:
		FunctionCache(size_t totalSize = 65536, float percentageKeep = 0.6);
		~FunctionCache();

		void * GetFunction(FunctionType type, const RasterizerState & state);

	private:
		void * AddFunction(FunctionType type, const RasterizerState & state, size_t size);
		void CompactCode();

	private:
		U8 *				m_Code;
		size_t				m_Used;
		size_t				m_Total;
		FunctionInfo *		m_Functions;
		FunctionInfo *		m_MostRecentlyUsed;
		FunctionInfo *		m_LeastRecentlyUsed;
		size_t				m_UsedFunctions;
		size_t				m_MaxFunctions;
		float				m_PercentageKeep;
	};
}


#endif //ndef EGL_FUNCTION_CACHE_H
