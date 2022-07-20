#查找当前路径下的所有代码文件,不递归查找
FILE(GLOB SRC_FILES "*.cpp" "*.c" "*.h")
#提取出当前文件夹的名称
STRING( REGEX REPLACE ".*/(.*)" "\\1" CURRENT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} )
#message("${SRC_FILES}")

#message("noUi=${noUi}")
#如果目录下存在代码文件
if(SRC_FILES)	
#包含配置文件
	include("subDirConfig.cmake")	
#设置生成的项目名称为当前目录的名称
	set(targetName ${CURRENT_FOLDER})
#添加包含目录
	include_directories(${station_dir})
	include_directories(${qzcv_dir})
	include_directories(${station_include})
#添加链接库路径，即station3生成的lib的路径，这个路径在station3下的CMakeLists.txt中定义
	link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
#开启多核心编译
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MP")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MP")
#添加宏定义到源文件的编译之中，相当于在源码中，敲上一句#define PRJ_NAME targetName
	add_compile_definitions(PRJ_NAME="${targetName}")
	
#判断是否定义了不生成ui,这个可能会在subDirConfig.cmake中
	if (NOT noUi)
#设置变量
		set(TS_FILES  "${targetName}_en.ts"  "${targetName}_zh.ts")
		set(qrcFile "${CMAKE_CURRENT_SOURCE_DIR}/${targetName}.qrc")
#如果目录下没有qrc文件，就从custom目录下,把那个subDir.qrc.in搞到当前路径下
#这个不是只复制文件，还会把里面的@targetName@替换成当前的targetName，最后一个变量不是@copyonly
		if(NOT EXISTS ${qrcFile})
			configure_file("${subDirQrcPattern}" "${qrcFile}" @ONLY)
		endif()
#如果生成的是station3模块
		if(targetType STREQUAL  "mod")
#算了，懒得写了，懂的都懂
			set(infoFile "${CMAKE_CURRENT_SOURCE_DIR}/${targetName}.ini")
			if(NOT EXISTS ${infoFile})
				configure_file("${subDirInfoPattern}" "${infoFile}" @ONLY)
			endif()
		endif()
#理论上，下面这两句，应该也要放到上面的那个if条件语句下的,ver版本文件应该是跟station3模块配置文件一样的
		set(versionFile "${CMAKE_CURRENT_SOURCE_DIR}/${targetName}.ver")
		if(NOT EXISTS ${versionFile})
			configure_file("${subDirVersionPattern}" "${versionFile}" @ONLY)
		endif()
#调用宏来生成qt的ui_*.h,moc_*.cpp文件
		qtGenerate(Generated_Files)
#设置TS_FILES的变量
		set(TS_FILES  "${targetName}_en.ts"  "${targetName}_zh.ts")
		set(trTargetName ${targetName}_tsfile)
#增加一个自定义的生成项目，专门用来生成这个项目的的ts文件和qm文件
		add_custom_target(${trTargetName} COMMAND ${Qt5_LUPDATE_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR} -ts 	${TS_FILES} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMAND ${Qt5_LRELEASE_EXECUTABLE} ${TS_FILES} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
#生成翻译文件的项目，统一分类到custom/translateProject
		set_target_properties(${trTargetName} PROPERTIES FOLDER "custom/${parentDirName}/translate")
#这里是判断有没有ts文件和qm文件，因为使用了自定义项目来生成翻译文件，在cmake的时候不会自动调用，在新项目中会因为没有ts和qm文件而报错
#所以需要在没有的时候，先生成出最开始的
		if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${targetName}_en.ts OR NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${targetName}_zh.ts)
			message(${targetName} " has not create translate files")
			execute_process(COMMAND ${qt_bin_path}/lupdate ${CMAKE_CURRENT_SOURCE_DIR} -ts ${TS_FILES} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
		endif()
		if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${targetName}_en.qm OR NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${targetName}_zh.qm)
			execute_process(COMMAND ${qt_bin_path}/lrelease ${TS_FILES} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
		endif()
		if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${targetName}_en.qm AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${targetName}_zh.qm)
			set(QM_FILES ${targetName}_en.qm ${targetName}_zh.qm)
		endif()
	endif()
	
#如果生成的是exe文件
	if(targetType STREQUAL  "app")
#生成的路径在Bin目录下
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Bin")
		message(STATUS "Create Custom application: ${targetName}")
#添加一个生成exe的项目，后面一堆是生成这个项目需要的文件
		add_executable(${targetName} ${SRC_FILES} ${Generated_Files} ${QM_FILES} ${TS_FILES}  ${versionFile}       "subDirConfig.cmake")
#设置vs的调试包含路径，相当于在vs属性页->调试->环境中PATH=xxx
		set_target_properties(${targetName} PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=${OpenCV_bin_path};${qt_bin_path};$(PATH)")
		set_target_properties(${targetName} PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "-develop")
#如果生成的是动态库
	elseif(targetType STREQUAL  "lib")
#生成的路径在Bin目录下
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Bin")
		message(STATUS "Create Custom dynamic libary: ${targetName}")
#添加一个生成动态库的项目，后面一堆是生成这个项目需要的文件
		if (NOT noUi)
			add_library(${targetName} SHARED ${SRC_FILES} ${Generated_Files} ${QM_FILES}  ${TS_FILES} ${versionFile}   "subDirConfig.cmake")
		else()
			add_library(${targetName} SHARED ${SRC_FILES} "subDirConfig.cmake")
		endif()
	elseif(targetType STREQUAL "nocreate")
		return()
	else()
#设置生成的路径是custom modules
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/custom modules")
		message(STATUS "Create Custom plugin module: ${targetName}")
#设置一个生成动态库的项目，后面一堆是生成这个项目需要的文件
		add_library(${targetName} SHARED ${SRC_FILES} ${Generated_Files} ${QM_FILES} ${TS_FILES}  ${infoFile} ${versionFile}    "subDirConfig.cmake")
	endif()
#把这个项目归类到vs的custom路径下
	set_target_properties(${targetName} PROPERTIES FOLDER "custom/${parentDirName}")
	if (NOT noUi)
#如果生成了ui文件，就把文件进行归类
		source_group("Generated Files" FILES ${Generated_Files})
		source_group("Form QRC Translate" FILES ${TS_FILES} ${QM_FILES}  ${infoFile} ${versionFile} )
	endif()
#添加链接库station和qzcv
	if(NOT noQZCV)
		target_link_libraries(${targetName} qzcv)
	endif()
	if(NOT noStation)
		target_link_libraries(${targetName} station)
	endif()
else()
#如果该路径下没有源代码文件，就查找子目录,递归进行同样的操作
	message(STATUS "${CURRENT_FOLDER} is a parent dir without source files")
	if(parentDirName)
		set(parentDirName "${parentDirName}/${CURRENT_FOLDER}")
	else()
		set(parentDirName ${CURRENT_FOLDER})
	endif()
	SUBDIRLIST(children ${CMAKE_CURRENT_SOURCE_DIR})
	foreach(child ${children})
		if(${child} STREQUAL   ".git")
			continue()
		endif()
		configure_file("${subDirCmakeFilePath}"  "${CMAKE_CURRENT_SOURCE_DIR}/${child}/CMakeLists.txt" COPYONLY)
		set(subConfigFile "${CMAKE_CURRENT_SOURCE_DIR}/${child}/subDirConfig.cmake")
		if(NOT EXISTS ${subConfigFile})
			configure_file("${subDirConfigPattern}" "${subConfigFile}" COPYONLY)
		endif()
		add_subdirectory(${child})
	endforeach()
endif()



