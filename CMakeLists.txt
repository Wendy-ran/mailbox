cmake_minimum_required(VERSION 3.14)
project(mailbox VERSION 0.1 LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 尝试找到 Qt6 先，如果找不到，则尝试 Qt5
find_package(Qt6 COMPONENTS Core Gui Widgets Network Sql QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 5.15 COMPONENTS Core Gui Widgets Network Sql REQUIRED)
    set(QT_VERSION_MAJOR 5)
else()
    set(QT_VERSION_MAJOR 6)
endif()

set(CMAKE_AUTOMOC ON)

# 如果你需要 Crypto++ 库，确保 vcpkg 已经设置好并且可以找到
# find_package(CryptoPP CONFIG REQUIRED)

# 源文件
set(SOURCE_FILES
    main.cpp
    mainwindow.cpp
    newdatabase.cpp
    newsocket.cpp
    objectpool.cpp
    tdialoglogin.cpp
    tformdoc.cpp
)

# 头文件
set(HEADER_FILES
    mainwindow.h
    newdatabase.h
    newsocket.h
    objectpool.h
    tdialoglogin.h
    tformdoc.h
)

# UI 文件
set(UI_FILES
    mainwindow.ui
    tdialoglogin.ui
    tformdoc.ui
)

# 资源文件
set(RESOURCE_FILES
    res.qrc
)

# 创建可执行文件
add_executable(${PROJECT_NAME} 
    ${SOURCE_FILES} 
    ${HEADER_FILES} 
    ${UI_FILES} 
    ${RESOURCE_FILES}
)

# 链接 Qt5 库
if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network Qt6::Sql
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network Qt5::Sql
    )
endif()

# 包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    # 如果你有其他的 include 目录，添加在这里
    # "${CMAKE_CURRENT_SOURCE_DIR}/path/to/includes"
)

# 安装规则，根据你的需求修改
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static
)
