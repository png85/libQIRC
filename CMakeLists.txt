#
# libQIRC: CMakeLists.txt
#

project(libQIRC)
cmake_minimum_required(VERSION 2.8)

#
# look for Qt4
#
find_package(Qt4 REQUIRED)

# We need the Qt network libraries for socket classes etc.
set(QT_USE_QTNETWORK TRUE)

# Prepare QT_LIBRARIES macro etc.
include(${QT_USE_FILE})

#
# list of libQIRC sources
set(libQIRC_SOURCES serverinfo.cc)

#
# list of libQIRC headers
set(libQIRC_HEADERS serverinfo.h ServerInfo)

#
# build target for libQIRC static library
add_library(QIRC ${libQIRC_SOURCES})

#
# install rules for library+headers
install(TARGETS QIRC ARCHIVE DESTINATION lib)
install(FILES ${libQIRC_HEADERS} DESTINATION include/QIRC)
