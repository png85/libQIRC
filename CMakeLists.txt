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


