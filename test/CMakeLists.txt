#aux_source_directory() 是一个常用命令，用于将某个目录下的所有源文件添加到一个变量中。
aux_source_directory(. SRC_LIST)

link_directories(${CMAKE_SOURCE_DIR}/lib)
# 编译动态库
# add_library(MuduoSimplify SHARED ${SRC_LIST})
add_executable(threadTest ${SRC_LIST})

# 动态库  链接 Muduo 库   target_link_libraries 是用来链接编译目标和

target_link_libraries(threadTest PRIVATE ThreadPool pthread)  
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR})    