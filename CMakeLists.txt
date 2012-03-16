#
# libQIRC: CMakeLists.txt
#

project(libQIRC)
cmake_minimum_required(VERSION 2.8)

#
# add cmake/ directory with our custom cmake macros to module path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

#
# Include CPack to build binary packages
set(CPACK_PACKAGE_DESCRIPTION "QIRC class library")
set(CPACK_PACKAGE_VENDOR "#das-system")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "99")
set(CPACK_PACKAGE_VERSION_PATCH "6")
set(CPACK_GENERATOR "TGZ;TBZ2")
include(CPack)

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
set(libQIRC_SOURCES serverinfo.cc hostmask.cc connection.cc)

#
# list of libQIRC headers
set(libQIRC_HEADERS serverinfo.h ServerInfo
  hostmask.h HostMask connection.h Connection)

# list of headers to process with Qt moc
set(libQIRC_MOC_HEADERS connection.h)
QT4_WRAP_CPP(libQIRC_MOC_SOURCES ${libQIRC_MOC_HEADERS})

#
# build target for libQIRC static library
add_library(QIRC ${libQIRC_SOURCES} ${libQIRC_MOC_SOURCES})

#
# install rules for library+headers
install(TARGETS QIRC ARCHIVE DESTINATION lib)
install(FILES ${libQIRC_HEADERS} DESTINATION include/QIRC)

#
# Check if Doxygen is installed and if so add build targets for
# the API dox
include(DocTarget OPTIONAL)

#
# Install a CMake macro to find our library
install(FILES FindQIRC.cmake DESTINATION share/QIRC)
