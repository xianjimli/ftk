INCLUDES = -I$(abs_top_srcdir)/opengles/include -I$(abs_top_srcdir)/opengles/src -I$(abs_top_srcdir)/opengles/src/linux-ftk \
	-I$(abs_top_srcdir)/opengles/src/arm -I$(abs_top_srcdir)/opengles/src/codegen \
	-I$(abs_top_srcdir)/src -I$(abs_top_srcdir)/src/im  -I$(abs_top_srcdir)/src/os/linux -I$(abs_top_srcdir)/ \
	-DUSE_FTK

lib_LTLIBRARIES = libopengles.la
libopengles_la_LDFLAGS=-lpthread
libopengles_la_SOURCES =  \
        $(srcdir)/Arrays.h \
        $(srcdir)/Buffer.h \
        $(srcdir)/Color.h \
        $(srcdir)/Config.cpp \
        $(srcdir)/Config.h \
        $(srcdir)/Context.cpp \
        $(srcdir)/Context.h \
        $(srcdir)/ContextBuffer.cpp \
        $(srcdir)/ContextFloat.cpp \
        $(srcdir)/ContextLight.cpp \
        $(srcdir)/ContextLines.cpp \
        $(srcdir)/ContextMaterial.cpp \
        $(srcdir)/ContextMatrix.cpp \
        $(srcdir)/ContextMesh.cpp \
        $(srcdir)/ContextPoints.cpp \
        $(srcdir)/ContextRaster.cpp \
        $(srcdir)/ContextRender.cpp \
        $(srcdir)/ContextTexture.cpp \
        $(srcdir)/ContextTriangles.cpp \
        $(srcdir)/ContextViewport.cpp \
        $(srcdir)/Display.cpp \
        $(srcdir)/Display.h \
        $(srcdir)/FractionalColor.h \
        $(srcdir)/Light.cpp \
        $(srcdir)/Light.h \
        $(srcdir)/LineClipper.inc \
        $(srcdir)/Material.cpp \
        $(srcdir)/Material.h \
        $(srcdir)/MatrixStack.cpp \
        $(srcdir)/MatrixStack.h \
        $(srcdir)/OGLES.h \
        $(srcdir)/Rasterizer.cpp \
        $(srcdir)/Rasterizer.h \
        $(srcdir)/RasterizerState.cpp \
        $(srcdir)/RasterizerState.h \
        $(srcdir)/RasterizerTriangles.cpp \
        $(srcdir)/RasterizerTriangles.inc \
        $(srcdir)/Texture.cpp \
        $(srcdir)/Texture.h \
        $(srcdir)/TriangleClipper.inc \
        $(srcdir)/TriangleClipperHigh.inc \
        $(srcdir)/TriangleClipperLow.inc \
        $(srcdir)/Types.h \
        $(srcdir)/Utils.cpp \
        $(srcdir)/Utils.h \
        $(srcdir)/fixed.cpp \
        $(srcdir)/fixed.h \
        $(srcdir)/gl.cpp \
        $(srcdir)/gl_sin.h \
        $(srcdir)/linalg.cpp \
        $(srcdir)/linalg.h \
        $(srcdir)/arm/CodeGenerator.cpp \
        $(srcdir)/arm/CodeGenerator.h \
        $(srcdir)/arm/FunctionCache.cpp \
        $(srcdir)/arm/FunctionCache.h \
        $(srcdir)/arm/GenFragment.cpp \
        $(srcdir)/arm/GenLline.cpp \
        $(srcdir)/arm/GenPoint.cpp \
        $(srcdir)/arm/GenScanline.cpp \
        $(srcdir)/arm/GenTriangle.cpp \
        $(srcdir)/codegen/arm-codegen.c \
        $(srcdir)/codegen/arm-codegen.h \
        $(srcdir)/codegen/arm-dis.c \
        $(srcdir)/codegen/arm-dis.h \
        $(srcdir)/codegen/arm_dpimacros.h \
        $(srcdir)/codegen/bitset.c \
        $(srcdir)/codegen/bitset.h \
        $(srcdir)/codegen/codegen.h \
        $(srcdir)/codegen/emit.c \
        $(srcdir)/codegen/emit.h \
        $(srcdir)/codegen/heap.c \
        $(srcdir)/codegen/heap.h \
        $(srcdir)/codegen/instruction.c \
        $(srcdir)/codegen/instruction.h \
        $(srcdir)/codegen/segment.c \
        $(srcdir)/codegen/segment.h \
        $(srcdir)/linux-ftk/ContextLinux.cpp \
        $(srcdir)/linux-ftk/Surface.cpp \
        $(srcdir)/linux-ftk/Surface.h \
        $(srcdir)/linux-ftk/e32def.h \
        $(srcdir)/linux-ftk/egl.cpp \
        $(srcdir)/linux-ftk/stdafx.cpp \
        $(srcdir)/linux-ftk/stdafx.h \
        $(srcdir)/../ug/ug_ftk.c  \
        $(srcdir)/../ug/uglu.c \
        $(srcdir)/../ug/ugshape.c \
        $(srcdir)/../ug/gl_sin.h

