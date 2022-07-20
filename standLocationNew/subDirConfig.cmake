#子项目的配置信息文件,会被子项目的CMakeLists.txt所包含
set(Halcon_PATH "C:/Program Files/MVTec/HALCON-10.0" CACHE PATH "halcon path")

#set target type
#app: application exe; lib: dynamci libary; mod: plugin module; 
set(targetType mod)

#set whether ui is needed
set(noUi OFF)

#add definition of Pre-compiled
#Example:  add_definitions(-Dmy_DEFINE) 
#means add definition my_DEFINE

#add_definitions(-Dmy_DEFINE) 

#add include directory 
#Example: include_directories("C:/my/include")
# means add include "C:/my/include"

include_directories("${Halcon_PATH}/include")
include_directories("${Halcon_PATH}/include/cpp")
include_directories("${CMAKE_SOURCE_DIR}/misc/halOpenCV")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../chipClass")

#include_directories("C:/my/include")
#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../demoMod")
#include_directories("${CMAKE_SOURCE_DIR}/common/match")

#add directory link libraries
#Example: link_directories("C:/my/lib")
#means add libary directory:  "C:/my/lib"

#link_directories("C:/my/lib")

set(HalconLinkDir "${Halcon_PATH}/lib/x86sse2-win32")
link_directories("${HalconLinkDir}")

#add link librarys
#Example: link_libraries("mylib")
#means add libary "mylib.lib"

#link_libraries("mylib")

link_libraries("halconcpp1.lib")
link_libraries("halOpenCV.lib")
link_libraries("chipClass.lib")

if(NOT EXISTS ${HalconLinkDir}/halconcpp1.lib)
	message(FATAL_ERROR "halconcpp1.lib not found, check Halcon_PATH")
endif()