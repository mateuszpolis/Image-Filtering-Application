cmake_minimum_required(VERSION 3.16)
project(ImageFiltering VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/imageprocessor.cpp
    src/filters/functionfilters.cpp
    src/filters/convolutionfilters.cpp
)

set(HEADERS
    include/mainwindow.h
    include/imageprocessor.h
    include/filters/functionfilters.h
    include/filters/convolutionfilters.h
)

set(UI_FILES
    resources/mainwindow.ui
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${UI_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Copy resources to build directory
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR}) 