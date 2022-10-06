cmake_minimum_required(VERSION 2.6)

PROJECT(ZH_SERVER_PROJECT)


# 使用add_subdirectory调用子目录的MakeLists.txt
add_subdirectory(test_staticlib)
add_subdirectory(test_dynamiclib)
add_subdirectory(LXMysql)
add_subdirectory(XAgent)
add_subdirectory(XCenter)
add_subdirectory(XClient)
add_subdirectory(main)