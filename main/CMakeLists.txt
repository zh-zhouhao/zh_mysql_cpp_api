PROJECT(client)

# 生成可调试执行文件
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

ADD_DEFINITIONS( -g -W -Wall -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DAC_HAS_INFO
-DAC_HAS_WARNING -DAC_HAS_ERROR -DAC_HAS_CRITICAL -DTIXML_USE_STL
-DAC_HAS_DEBUG -DLINUX_DAEMON -D_GLIBCXX_USE_NANOSLEEP -std=c++11)

AUX_SOURCE_DIRECTORY(./ SRC_LIST)           # 把源文件放在 SRC_LIST 变量中

SET(EXECUTABLE_OUTPUT_PATH  ${ZH_SERVER_PROJECT_SOURCE_DIR}/main/_bin)             # 指定可执行文件的输出路径

## 静态库 ##
# ##################添加头文件路径和库路径的方法1#######################################
# ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_LIST})   # 执行输出可执行文件

# target_include_directories( ${PROJECT_NAME}
#     PUBLIC ${ZH_SERVER_PROJECT_SOURCE_DIR}/test_staticlib/_include
# )
# target_link_libraries(${PROJECT_NAME}
#     ${ZH_SERVER_PROJECT_SOURCE_DIR}/test_staticlib/_lib/libtest_staticlib.a
# )
# ##################################################################################

##################添加头文件路径和库路径的方法2#######################################
# INCLUDE_DIRECTORIES(${ZH_SERVER_PROJECT_SOURCE_DIR}/test_staticlib/_include/)
# LINK_DIRECTORIES( ${ZH_SERVER_PROJECT_SOURCE_DIR}/test_staticlib/_lib/) 
# # # PS：这LINK_DIRECTORIES要放在ADD_EXECUTABLE之前

# ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_LIST})   # 执行输出可执行文件

# target_link_libraries(${PROJECT_NAME}
#     libtest_staticlib.a
# )
##################################################################################

## 动态库 ##
##################################################################################
# INCLUDE_DIRECTORIES(${ZH_SERVER_PROJECT_SOURCE_DIR}/test_dynamiclib/_include/)
# LINK_DIRECTORIES( ${ZH_SERVER_PROJECT_SOURCE_DIR}/test_dynamiclib/_lib/) 
# # PS：这LINK_DIRECTORIES要放在ADD_EXECUTABLE之前

# ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_LIST})   # 执行输出可执行文件

# target_link_libraries(${PROJECT_NAME}
#     test_dynamiclib.so
# )
##################################################################################

## 动态库和静态库一起使用
##################################################################################
INCLUDE_DIRECTORIES(
    ${ZH_SERVER_PROJECT_SOURCE_DIR}/test_dynamiclib/_include/
    ${ZH_SERVER_PROJECT_SOURCE_DIR}/test_staticlib/_include/
    /usr/include/

    ${ZH_SERVER_PROJECT_SOURCE_DIR}/LXMysql/_include/
    ${ZH_SERVER_PROJECT_SOURCE_DIR}/XAgent/_include/
    ${ZH_SERVER_PROJECT_SOURCE_DIR}/XCenter/_include/
    ${ZH_SERVER_PROJECT_SOURCE_DIR}/XClient/_include/
)

LINK_DIRECTORIES( 
    ${ZH_SERVER_PROJECT_SOURCE_DIR}/test_staticlib/_lib/ 
    ${ZH_SERVER_PROJECT_SOURCE_DIR}/test_dynamiclib/_lib/
    /usr/lib/x86_64-linux-gnu

    ${ZH_SERVER_PROJECT_SOURCE_DIR}/LXMysql/_lib/
    ${ZH_SERVER_PROJECT_SOURCE_DIR}/XAgent/_lib/
    ${ZH_SERVER_PROJECT_SOURCE_DIR}/XCenter/_lib/
    ${ZH_SERVER_PROJECT_SOURCE_DIR}/XClient/_lib/
)

set(MYSQL_LIBS mysqlclient pthread)

ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(${PROJECT_NAME}
    libtest_staticlib.a
    test_dynamiclib.so
    ${MYSQL_LIBS}

    libXCenter.a
    libXClient.a

    #libXAgent.a和libLXMysql.so有先后顺序的
    libXAgent.a
    libLXMysql.so
    
)
##################################################################################