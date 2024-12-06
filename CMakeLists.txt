cmake_minimum_required(VERSION 3.15)

############################################################
# 用户设置部分
############################################################
set(PROJECT_USER_NAME bsdiff_upgrade)
set(USER_LANGUAGES C)#设置代码中的所有语言类型，如C CXX ASM
# set(PACKAGE_LIST cmsis-dsp)#本包依赖的包
############################################################
# Create a library
############################################################
# 查找当前目录下的所有源文件并将名称保存到 DIR_LIB_SRCS 变量
file(GLOB_RECURSE DIR_LIB_SRCS
	"${CMAKE_CURRENT_LIST_DIR}/*.c"
	)
#搜索后排除不编译的特定文件
# list(REMOVE_ITEM DIR_LIB_SRCS
#     "${CMAKE_CURRENT_LIST_DIR}/Source/TransformFunctions/TransformFunctionsF16.c"
# )

#设置交叉编译环境，必备，否则会出现丢失libxxx的情况
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# linux
if(CMAKE_HOST_UNIX) 
    set(CMAKE_C_COMPILER_WORKS ON)
    set(CMAKE_CXX_COMPILER_WORKS ON)
    set(CMAKE_ASM_COMPILER_WORKS ON)
endif()

project(${PROJECT_USER_NAME} LANGUAGES ${USER_LANGUAGES})

#取DIR_LIB_SRCS中的第一个文件获取文件夹
list(GET DIR_LIB_SRCS 0 FIRST_SRC)
get_filename_component(DIR_LIB_SRCS_DIR ${FIRST_SRC} DIRECTORY)
#获取conan路径
string(REGEX MATCH "([^/]+/[^/]+/[^/]+/[^/]+)" DIR_LIB_SRCS_DIR_CONAN ${DIR_LIB_SRCS_DIR})

#设置在python基础上增加的编译选项,重点是优化等级
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "IAR")# using IAR
    string(APPEND C_FLAGS " -Oh")
    string(APPEND CXX_FLAGS " -Oh")
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "ARMCC")# using ARMCC
    string(APPEND C_FLAGS " -O3")
    string(APPEND CXX_FLAGS " -O3")
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")# using GNU
    string(APPEND C_FLAGS " -O2 -ffile-prefix-map=${DIR_LIB_SRCS_DIR_CONAN}=.")
    string(APPEND CXX_FLAGS " -O2 -ffile-prefix-map=${DIR_LIB_SRCS_DIR_CONAN}=.")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(Build type is Debug.)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    string(APPEND C_FLAGS " -DNDEBUG")
    message(Build type is Release.)
endif()

message(${CMAKE_C_COMPILER_ID})

#优化等级
set(CMAKE_C_FLAGS_DEBUG "" CACHE STRING INTERNAL FORCE) 
set(CMAKE_C_FLAGS_RELEASE "" CACHE STRING INTERNAL FORCE)

# 当用了std LIB库的时候,需要处理包含编译器头文件路径
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "IAR")# using IAR
    #Set DLib_Config
    get_filename_component(COMPILER_DIR ${CMAKE_C_COMPILER} DIRECTORY)
	get_filename_component(COMPILER_DIR ${COMPILER_DIR} DIRECTORY)
    set(DLIB_CONFIG ${COMPILER_DIR}/inc/c/DLib_Config_Full.h)

    #linux下编译需要将路径转换为windows路径
    if(CMAKE_HOST_UNIX)
        execute_process (
            COMMAND wslpath -w ${DLIB_CONFIG}
            OUTPUT_VARIABLE DLIB_CONFIG
        )
        string(REPLACE "\n" "" DLIB_CONFIG ${DLIB_CONFIG}) #必须去掉换行符
    endif()
    #最终结果
    message(${DLIB_CONFIG})
    string(APPEND C_FLAGS " --dlib_config \"${DLIB_CONFIG}\"")
    string(APPEND CXX_FLAGS " --dlib_config \"${DLIB_CONFIG}\"")
endif()

SET(CMAKE_C_FLAGS ${C_FLAGS})
SET(CMAKE_CXX_FLAGS ${CXX_FLAGS})
SET(CMAKE_ASM_FLAGS ${ASM_FLAGS})

#Generate the static library from the library sources
add_library(${PROJECT_NAME} STATIC ${DIR_LIB_SRCS})

FOREACH(PACKAGE_LIST ${PACKAGE_LIST})
  message("Current search ${PACKAGE_LIST} package...") #循环取值
  find_package(${PACKAGE_LIST} CONFIG REQUIRED)
  # Global approach
  if(${PACKAGE_LIST}_FOUND)
        message(${${PACKAGE_LIST}_INCLUDE_DIRS})
          target_include_directories(${PROJECT_NAME} PRIVATE ${${PACKAGE_LIST}_INCLUDE_DIRS})
          target_link_libraries(${PROJECT_NAME}
          PRIVATE
          ${${PACKAGE_LIST}_LIB_DIRS_RELEASE}/lib${PACKAGE_LIST}.a
          )
  endif()
ENDFOREACH(PACKAGE_LIST)

#head file
file(GLOB_RECURSE Head_Files 
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
	)
#查找头文件路径
FOREACH(Head_Files ${Head_Files})
    string(REGEX REPLACE "(.*/)" "" HEAD_FILE ${Head_Files})#匹配文件名
    string(REPLACE ${HEAD_FILE} "" HEAD_FILE_DIR ${Head_Files})#去除文件名
    list(APPEND HEAD_INCLUDE_DIR ${HEAD_FILE_DIR})#添加到列表
ENDFOREACH(Head_Files)
#路径去重
list(REMOVE_DUPLICATES HEAD_INCLUDE_DIR)
#指定编译时包含的头文件路径
target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${HEAD_INCLUDE_DIR}
        )
message(${CMAKE_CURRENT_SOURCE_DIR})
#安装所有头文件
install(FILES ${Head_Files} DESTINATION include)
install(TARGETS ${PROJECT_NAME})
