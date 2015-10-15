INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_YSF ysf)

FIND_PATH(
    YSF_INCLUDE_DIRS
    NAMES ysf/api.h
    HINTS $ENV{YSF_DIR}/include
        ${PC_YSF_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    YSF_LIBRARIES
    NAMES gnuradio-ysf
    HINTS $ENV{YSF_DIR}/lib
        ${PC_YSF_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(YSF DEFAULT_MSG YSF_LIBRARIES YSF_INCLUDE_DIRS)
MARK_AS_ADVANCED(YSF_LIBRARIES YSF_INCLUDE_DIRS)

