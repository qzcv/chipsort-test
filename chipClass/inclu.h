#pragma once

#ifdef WIN32
#ifdef CHIP_EXPORTS
# define EXPORTS __declspec(dllexport)
#else
# define EXPORTS __declspec(dllimport)
#endif
#else
# define CHIP_EXPORTS
#endif