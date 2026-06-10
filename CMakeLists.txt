cmake_minimum_required(VERSION 3.16)
project(ComicReader VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

include_directories(include)

# Define your source and header files
set(SOURCES
    src/main.cpp
    src/ReaderWindow.cpp
    src/FolderReader.cpp
    include/ReaderWindow.h
    include/FolderReader.h
    include/ThumbnailDelegate.h
)

add_executable(ComicReader ${SOURCES})
target_link_libraries(ComicReader PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
