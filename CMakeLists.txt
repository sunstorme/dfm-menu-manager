cmake_minimum_required(VERSION 3.16)
project(dfm-menu-manager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Qt6 依赖
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Quick
    Qml
    LinguistTools
)

# 自动收集源文件（添加新文件时无需修改CMakeLists.txt）
file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

# 自动收集头文件（添加新文件时无需修改CMakeLists.txt）
file(GLOB_RECURSE HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

# 如需排除特定文件，取消注释以下行并修改正则表达式
# list(FILTER SOURCES EXCLUDE REGEX "${CMAKE_CURRENT_SOURCE_DIR}/src/excluded/*")

# 设置QML资源文件
set(QML_RESOURCES
    src/resources/qml.qrc
    src/resources/i18n.qrc
)

# 创建可执行文件
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${QML_RESOURCES}
)

# 链接库
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Qml
)

# 包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 翻译文件
set(TS_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/dfm-menu-manager_zh_CN.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/dfm-menu-manager_en_US.ts
)

# 添加自定义目标用于更新翻译
# 自动收集所有需要翻译的源文件
file(GLOB_RECURSE TRANSLATION_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/qml/*.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/qml/*.js
)

add_custom_target(lupdate
    COMMAND Qt6::lupdate ${TRANSLATION_SOURCES} -ts ${TS_FILES}
    COMMENT "Updating translation files from all sources"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
)

add_custom_target(lrelease ALL
    COMMAND Qt6::lrelease ${TS_FILES}
    COMMENT "Releasing translation files"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    BYPRODUCTS
        ${CMAKE_CURRENT_SOURCE_DIR}/translations/dfm-menu-manager_zh_CN.qm
        ${CMAKE_CURRENT_SOURCE_DIR}/translations/dfm-menu-manager_en_US.qm
)

# 安装规则
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# 测试
enable_testing()
add_subdirectory(tests)

