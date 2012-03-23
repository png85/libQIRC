if(QIRC_INCLUDE_DIR)
  set(QIRC_FIND_QUIETLY TRUE)
endif(QIRC_INCLUDE_DIR)

find_path(QIRC_INCLUDE_DIR QIRC/connection.h
  PATHS
  "$ENV{QIRC}/include"
  /usr/local/include
  /usr/pkg/include
  /usr/include
)

find_library(QIRC_LIBRARIES QIRC
  PATHS
  "$ENV{QIRC}/lib"
  /usr/local/lib
  /usr/pkg/lib
  /usr/lib
)

set(QIRC_FOUND FALSE)
if(QIRC_INCLUDE_DIR)
  if(QIRC_LIBRARIES)
    set(QIRC_FOUND TRUE)
    if(NOT QIRC_FIND_QUIETLY)
      message(STATUS "Found libQIRC: ${QIRC_LIBRARIES}")
    endif(NOT QIRC_FIND_QUIETLY)
  endif(QIRC_LIBRARIES)
endif(QIRC_INCLUDE_DIR)

mark_as_advanced(
  QIRC_INCLUDE_DIR
  QIRC_LIBRARIES
)