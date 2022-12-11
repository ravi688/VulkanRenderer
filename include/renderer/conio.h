
#pragma once

#include <renderer/defines.h>


BEGIN_CPP_COMPATIBLE

#if defined(RENDERER_PLATFORM_LINUX) || defined(RENDERER_PLATFORM_UNIX) || defined(RENDERER_PLATFORM_MACOS)
	// custom defined using ncurses
	RENDERER_API char cio_getch();
	RENDERER_API bool cio_kbhit();
	#define getch cio_getch
	#define kbhit cio_kbhit
#elif defined(RENDERER_PLATFORM_WINDOWS)
#	ifdef RENDERER_COMPILER_MINGW
#		include <conio.h>
		static FORCE_INLINE char cio_getch() { return getch(); }
		static FORCE_INLINE bool cio_kbhit() { return kbhit(); }
#	else
		// custom defined using ncurses
		RENDERER_API char cio_getch();
		RENDERER_API bool cio_kbhit();
		#define getch cio_getch
		#define kbhit cio_kbhit
#	endif // RENDERER_COMPILER_MINGW
#else
#	warning "Unrecognized platform you are compiling on, some features/functionalities might not work"
	static FORCE_INLINE char cio_getch() { return 0; }
	static FORCE_INLINE bool cio_kbit() { return false; }
	#define getch cio_getch
	#define kbhit cio_kbhit
#endif

END_CPP_COMPATIBLE
