Vincent 3D Rendering Library for Pocket PC
==========================================


http://sourceforge.net/projects/ogl-es/



Overview
--------

Vincent develops a compliant implementation of the OpenGL (R) ES 1.1 API specification for 
Pocket PCs and Smartphones. The current target platforms are Windows Mobile,Symbian and Linux 
devices using TI OMAP, Intel (R) StrongARM and XScale PA2xx processors.

As officially registered adopter of the OpenGL ES API, this project has full access to the
official OpenGL ES 1.1 conformance test. Starting with build 0.8, Vincent fully passes the 
conformance test for the Common Light profile, i.e. the fixed point API defined by the standard.

On January 25, 2006, results of executing the conformance test on the following configuration
have been submitted to Khronos:

Vincent OpenGL ES 1.1 Common Lite
---------------------------------

CPUs:		ARMv4

OS:		Windows Mobile 2003

API pipeline:	GL_VENDOR       "Hans-Martin Will"
		GL_RENDERER	"Software"
		GL_VERSION	"OpenGL ES-CL 1.1 Software JIT"
			
Display configuration:  48 x 48, 16bpp

As a note, even though Vincent implements the API defined by the common profile, it
is not implemented using floating point arithmetic, but rather converts floats to
fixed values through a compatibility layer. In external executions, the library
still passes all but one test for the common profile: The test that verifies 
retrieval of state variables. This test fails due to loss of precision due to internal storage
of certain parameters as fixed vs. floating point values.

Further builds of the project will focus on addressing performance issues present in the
current build, as well as adressing issues as reported by users that are not covered by
the tests we have run so far.




How the project is organized
----------------------------

This project has build configurations for the following configurations:

- Visual Studio.NET 2003 for IA32/Windows
- Embedded Visual C++ 4.0 SP3 (both Emulator, ARM and XScale) for PocketPC 2003 or Smartphone 2003
- GNU Autotools for ARM Linux

You will need at least one of these environments installed on your computer. Please refer
to the project home page for links on how to obtain these tools.

The project tree as checked into sourceforge provides everything you need to build the project. The
folder structure is as follows:

/src				- Source files
	/ARM			- Code generation for ARM
	/codegen		- A compiler backend library
	/epoc			- Symbian platform files (not really maintained at this point)
	/WinCE			- Windows Mobile platform files
	/linux-x11		- Linux platform files (uses X11)

/projects			- Visual Studio project files
	/evc4			- for embedded Visual C++ 4.0
	/epoc			- for Symbian development 
	/linux-x11		- for Linux development 

/obj				- Intermediate build files
	/arm			- for ARM
		/Debug
		/Release
	/emu			- for PocketPC emulator
		/Debug
		/Release
	/xscale			- for Intel XScale
		/Debug
		/Release

/bin				- Final binaries
	/arm
		/Debug
		/Release
	/emu
		/Debug
		/Release
	/xscale
		/Debug
		/Release

/include
	/GLES			- The OpenGL ES library header files
	
/redbook			- Conversion of the redbook examples
	/evc4			- A project workspace for embedded Visual C++ 4.0
	
/ug
				- A helper library that provides a subset of GLUT for Pocket PCs
	

/doc				- Various documents that are helpful for this project

/tools				- Build scripts and helper tools


How do I get started?
---------------------

Check out the "Getting Started" page on SourceForge: http://ogl-es.sourceforge.net/getting_started.htm
To start programming with OpenGL ES the redbook examples that are provided with this project
make a good starting point.


How do I build this project from the source?
--------------------------------------------

To build the libraries, open the corresponding project/workspace from the projects
subtree. If the download is correct, the project should build right away without 
any further adjustment. For Linux compilations please refer to doc/readme-linux.txt
for instructions.

The file OGLES.h contains several configuration parameters that you can use to
fine-tine the library build. The most interesting parameters are:

EGL_NUM_TEXTURE_UNITS		default: 2	the number of texture units; OpenGL ES 1.0 requires >= 1,
						OpenGL ES 1.1 requires >= 2
						
EGL_MIPMAP_PER_TEXEL		default: 1	should mipmap selection happen on a per-pixel level?
						1 is for full conformance, 0 uses mipmap selection per
						primitive instead, and hence won't fully pass the 
						conformance test.

How do I get support?
---------------------

Please refer to the SourceForge project homepage on how to obtain support. Preferably, please enter a
support request through the SourceForge project page. 


How can I contribute?
---------------------

Please refer to the SourceForge project homepage on how to contribute to this project.

