#include "ftk.h"
#include <dlfcn.h>

typedef Ret (*FtkMain)(int argc, char* argv[]);

static Ret launch_app(const char* sharelib, const char* function, int argc, char* argv[])
{
	void* handle = dlopen(sharelib, RTLD_NOW);
	if(handle != NULL)
	{
		FtkMain entry = (FtkMain)dlsym(handle, function);
		if(entry != NULL)
		{
			return entry(argc, argv);
		}
		else
		{
			ftk_logd("dlsym %s failed.\n", function);
		}
	}
	else
	{
		ftk_logd("dlopen %s failed.\n", sharelib);
	}

	return RET_FAIL;
}

int main(int argc, char* argv[])
{
	const char* sharelib = NULL;
	const char* function = NULL;
	if(argc < 2)
	{
		ftk_logd("Usage: %s sharelib [function (deault use ftk_main)]\n", argv[0]);

		return 0;
	}

	sharelib = argv[1];
	function = argv[2] != NULL ? argv[2] : "ftk_main";
	ftk_init(argc, argv);

	if(launch_app(sharelib, function, argc, argv) == RET_OK)
	{
		ftk_run();
	}
	else
	{
		ftk_loge("launch %s %s failed.\n", sharelib, function);
	}

	return 0;
}
