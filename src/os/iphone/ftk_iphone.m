
#import <signal.h>
#import "ftk_typedef.h"
#import "ftk_iphone_view.h"

#ifdef HAVE_EXECINFO_H
#import <execinfo.h>
#endif

static void signal_handler(int sig)
{
#ifdef HAVE_EXECINFO_H
	int i = 0;
	void* buffer[100] = {0};
	int n = backtrace(buffer, 100);
	const char** symbols = (const char**)backtrace_symbols(buffer, n);

	for(i = 0; i < n; i++)
	{
		printf("%p: %s\n", buffer[i], symbols[i]);
	}

	fflush(stdout);

	signal(SIGABRT, NULL);
	signal(SIGSEGV, NULL);
	signal(SIGILL,  NULL);
#endif
	_exit(0);

	return;
}

static void signal_int_handler(int sig)
{
	exit(0);

	return;
}

static void ftk_install_crash_signal(void)
{
	signal(SIGABRT, signal_handler);
	signal(SIGSEGV, signal_handler);
	signal(SIGILL,  signal_handler);
	signal(SIGINT,  signal_int_handler);

	return ;
}

static int ftk_set_tty_mode(int graphics)
{
	int r = 0;
#if !defined(PC_EMU) && defined(USE_LINUX_NATIVE)
	int fd = 0;
	fd = open("/dev/tty0", O_RDWR | O_SYNC);
	if(fd < 0)
	{
		return -1;
	}
	r = ioctl(fd, KDSETMODE, (void*) (graphics ? KD_GRAPHICS : KD_TEXT));
	close(fd);
#endif
	return r;
}

size_t ftk_get_relative_time(void)
{
	struct timeval now = {0};
	gettimeofday(&now, NULL);

	return now.tv_sec*1000 + now.tv_usec/1000;
}

int ftk_platform_init(int argc, char** argv)
{
	ftk_set_tty_mode(1);
	ftk_install_crash_signal();

	return RET_OK;
}

void ftk_platform_deinit(void)
{
	ftk_set_tty_mode(0);

	return;
}


static int os_argc;
static char** os_argv;
static jmp_buf jmp_env;

jmp_buf* jump_env(void)
{
	return &jmp_env;
}

@interface FtkAppDelegate : NSObject<UIApplicationDelegate>
{
}
@end

@implementation FtkAppDelegate

-(id)init
{
	self = [super init];
	return self;
}

-(void)dealloc
{
	[super dealloc];
}

-(void)postFinishLaunch
{
	int rc, i;

	rc = FTK_MAIN(os_argc, os_argv);

	for(i = 0; i < os_argc; i++)
	{
		free(os_argv[i]);
	}

	free(os_argv);

	ftk_iphone_view_destroy_win();

	ftk_logd("%s: exit\n", __func__);

	exit(rc);
}

-(void)applicationDidFinishLaunching:(UIApplication *)application
{
	[[NSFileManager defaultManager] changeCurrentDirectoryPath:[[NSBundle mainBundle] resourcePath]];

	ftk_iphone_view_create_win();

	[self performSelector:@selector(postFinishLaunch) withObject:nil afterDelay:0.0];
}

-(void)applicationWillTerminate:(UIApplication *)application
{
	FTK_QUIT();
	longjmp(*(jump_env()), 1);
}

-(void)applicationWillResignActive:(UIApplication *)application
{
}

-(void)applicationDidBecomeActive:(UIApplication *)application
{
}

@end

int main(int argc, char* argv[])
{
	int rc, i;
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

	os_argc = argc;

	os_argv = (char **) malloc((argc + 1) * sizeof(char *));
	if(os_argv == NULL)
	{
		return 1;
	}

	for(i = 0; i < argc; i++)
	{
		os_argv[i] = malloc(strlen(argv[i]) + 1);
		if(os_argv[i] == NULL)
		{
			return 1;
		}

		strcpy(os_argv[i], argv[i]);
	}

	os_argv[i] = NULL;

	rc = UIApplicationMain(argc, argv, NULL, @"FtkAppDelegate");

	[pool release];

	return rc;
}
