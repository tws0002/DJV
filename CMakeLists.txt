cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0074 NEW)

project(DJV)

set(DJV_VERSION_MAJOR "1")
set(DJV_VERSION_MINOR "2")
set(DJV_VERSION_PATCH "2")
set(DJV_VERSION ${DJV_VERSION_MAJOR}.${DJV_VERSION_MINOR}.${DJV_VERSION_PATCH})

# Set the platform we are building for.
if(APPLE)
    add_definitions(-DDJV_OSX)
elseif(UNIX)
    string(COMPARE EQUAL ${CMAKE_SYSTEM_NAME} Linux _TMP)
    if(_TMP)
        add_definitions(-DDJV_LINUX)
        set(CMAKE_C_FLAGS -fPIC)
        set(CMAKE_CXX_FLAGS -fPIC)
        set(CMAKE_SKIP_BUILD_RPATH FALSE)
        set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
        # \bug This is hard-coded for CentOS.
        set(CMAKE_INSTALL_RPATH "/usr/lib")
    endif()
elseif(WIN32)
    add_definitions(-DDJV_WINDOWS)
endif()

# Update the CMake module path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake/Modules)

# Set the output directories for the build.
set(DJV_BUILD_DIR ${PROJECT_BINARY_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DJV_BUILD_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${DJV_BUILD_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${DJV_BUILD_DIR}/bin)

# Set the installation directories.
set(DJV_INSTALL_BIN bin)
set(DJV_INSTALL_LIB lib)
set(DJV_INSTALL_PLUGINS plugins)
set(DJV_INSTALL_DOC doc)
set(DJV_INSTALL_TRANSLATIONS translations)

# Set miscellaneous options.
add_definitions(-DDJV_MMAP)
add_definitions(-DDJV_ASSERT)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set the configuration file.
configure_file(
    ${PROJECT_SOURCE_DIR}/djvConfig.h.in
    ${PROJECT_BINARY_DIR}/djvConfig.h
    @ONLY)

# Enable testing.
enable_testing()

# Enable gcov code coverage targets.
set(DJV_GCOV false)

# Set compiler specific options.
if(UNIX)
    if(CMAKE_COMPILER_IS_GNUCXX)
        # Disable warnings from XPM files.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings")
        
        # Disable warnings from printf/sprintf.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-format-security")

        # Set profiling options.    
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
        #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")

        # Set gcov options.
        if(DJV_GCOV)
            include(CodeCoverage)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -ftest-coverage")
            set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs")
            set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -ftest-coverage")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -ftest-coverage")
        endif()
    endif()
endif()

# Include directories.
include_directories(
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/bin
    ${CMAKE_SOURCE_DIR}/tests
    ${CMAKE_BINARY_DIR})

# Third party libraries.
if(DJV_THIRD_PARTY)
    set(ZLIB_ROOT ${DJV_THIRD_PARTY})
    set(GLM_ROOT ${DJV_THIRD_PARTY})
    set(OpenAL_ROOT ${DJV_THIRD_PARTY})
    set(JPEG_ROOT ${DJV_THIRD_PARTY})
    set(TIFF_ROOT ${DJV_THIRD_PARTY})
    set(PNG_ROOT ${DJV_THIRD_PARTY})
    set(IlmBase_ROOT ${DJV_THIRD_PARTY})
    set(OpenEXR_ROOT ${DJV_THIRD_PARTY})
    set(FFmpeg_ROOT ${DJV_THIRD_PARTY})    
    set(Qt5_ROOT ${DJV_THIRD_PARTY})
    
    set(ZLIB_SHARED_LIBS ${BUILD_SHARED_LIBS})
    set(GLM_SHARED_LIBS ${BUILD_SHARED_LIBS})
    set(OPENAL_SHARED_LIBS ON)
    set(JPEG_SHARED_LIBS ${BUILD_SHARED_LIBS})
    set(TIFF_SHARED_LIBS ${BUILD_SHARED_LIBS})
    set(PNG_SHARED_LIBS ${BUILD_SHARED_LIBS})
    set(ILMBASE_SHARED_LIBS ${BUILD_SHARED_LIBS})
    set(OPENEXR_SHARED_LIBS ${BUILD_SHARED_LIBS})
    set(FFMPEG_SHARED_LIBS ON)
    set(QT_SHARED_LIBS ON)
endif()

find_package(Qt5 REQUIRED)

include(djvPackage)

add_subdirectory(bin)
add_subdirectory(lib)
add_subdirectory(tests)
add_subdirectory(examples)
add_subdirectory(experiments)
add_subdirectory(doc)

include(CPack)

