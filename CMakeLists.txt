#
# libQIRC: CMakeLists.txt
#

project(libQIRC)
cmake_minimum_required(VERSION 2.8)

#
# allow the user to select wether he wants a static or shared library
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
set(LIB_TYPE STATIC)
if(BUILD_SHARED_LIBS)
  set(LIB_TYPE SHARED)
endif(BUILD_SHARED_LIBS)

#
# add cmake/ directory with our custom cmake macros to module path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

#
# Include CPack to build binary packages
set(CPACK_PACKAGE_DESCRIPTION "QIRC class library")
set(CPACK_PACKAGE_VENDOR "#das-system")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "99")
set(CPACK_PACKAGE_VERSION_PATCH "10")
set(CPACK_GENERATOR "TGZ;TBZ2")
include(CPack)

#
# look for Qt4
#
find_package(Qt4 REQUIRED QtCore QtNetwork)

# Prepare QT_LIBRARIES macro etc.
include(${QT_USE_FILE})

#
# list of libQIRC sources
set(libQIRC_SOURCES serverinfo.cc hostmask.cc connection.cc colors.cc)

#
# list of libQIRC headers
set(libQIRC_HEADERS serverinfo.h ServerInfo
  hostmask.h HostMask connection.h Connection qirc.h)

# list of headers to process with Qt moc
set(libQIRC_MOC_HEADERS connection.h)
QT4_WRAP_CPP(libQIRC_MOC_SOURCES ${libQIRC_MOC_HEADERS})

#
# build target for libQIRC static library
add_library(QIRC ${LIB_TYPE} ${libQIRC_SOURCES} ${libQIRC_MOC_SOURCES})
target_link_libraries(QIRC ${QT_LIBRARIES})

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
