find_package(IlmBase REQUIRED)
find_package(OpenGL REQUIRED)

find_package(JPEG)
find_package(PNG)
find_package(TIFF)
find_package(OpenEXR)
find_package(FFmpeg)

set(header
    Cineon.h
    CineonHeader.h
    CineonLoad.h
    CineonPlugin.h
    CineonSave.h
    Color.h
    ColorInline.h
    ColorUtil.h
    ColorUtilInline.h
    ColorProfile.h
    DPX.h
    DPXHeader.h
    DPXLoad.h
    DPXPlugin.h
    DPXSave.h
    GraphicsContext.h
    Graphics.h
    IFF.h
    IFFLoad.h
    IFFPlugin.h
    IFFSave.h
    IFL.h
    IFLLoad.h
    IFLPlugin.h
    Image.h
    ImageIO.h
    ImageTags.h
    ImageUtil.h
    LUT.h
    LUTLoad.h
    LUTPlugin.h
    LUTSave.h
    OpenGL.h
    OpenGLImage.h
    OpenGLOffscreenBuffer.h
    OpenGLLUT.h
    OpenGLShader.h
    OpenGLTexture.h
    PIC.h
    PICLoad.h
    PICPlugin.h
    Pixel.h
    PixelData.h
    PixelDataInline.h
    PixelDataUtil.h
    PixelInline.h
    PPM.h
    PPMLoad.h
    PPMPlugin.h
    PPMSave.h
    RLA.h
    RLALoad.h
    RLAPlugin.h
    SGI.h
    SGILoad.h
    SGIPlugin.h
    SGISave.h
    Targa.h
    TargaLoad.h
    TargaPlugin.h
    TargaSave.h)
set(mocHeader
    ColorProfile.h
    ImageIO.h
    ImageTags.h
    OpenGLImage.h
    PixelData.h
    Pixel.h)
set(source
    Cineon.cpp
    CineonHeader.cpp
    CineonLoad.cpp
    CineonPlugin.cpp
    CineonSave.cpp
    Color.cpp
    ColorUtil.cpp
    ColorProfile.cpp
    DPX.cpp
    DPXHeader.cpp
    DPXLoad.cpp
    DPXPlugin.cpp
    DPXSave.cpp
    GraphicsContext.cpp
    IFF.cpp
    IFFLoad.cpp
    IFFPlugin.cpp
    IFFSave.cpp
    IFL.cpp
    IFLLoad.cpp
    IFLPlugin.cpp
    Image.cpp
    ImageIO.cpp
    ImageTags.cpp
    ImageUtil.cpp
    LUT.cpp
    LUTLoad.cpp
    LUTPlugin.cpp
    LUTSave.cpp
    OpenGL.cpp
    OpenGLImage.cpp
    OpenGLImageDraw.cpp
    OpenGLOffscreenBuffer.cpp
    OpenGLLUT.cpp
    OpenGLShader.cpp
    OpenGLTexture.cpp
    PIC.cpp
    PICLoad.cpp
    PICPlugin.cpp
    Pixel.cpp
    PixelConvert.cpp
    PixelData.cpp
    PixelDataUtil.cpp
    PPM.cpp
    PPMLoad.cpp
    PPMPlugin.cpp
    PPMSave.cpp
    RLA.cpp
    RLALoad.cpp
    RLAPlugin.cpp
    SGI.cpp
    SGILoad.cpp
    SGIPlugin.cpp
    SGISave.cpp
    Targa.cpp
    TargaLoad.cpp
    TargaPlugin.cpp
    TargaSave.cpp)
if(JPEG_FOUND)
    set(header
        ${header}
        JPEG.h
        JPEGLoad.h
        JPEGPlugin.h
        JPEGSave.h)
    set(source
        ${source}
        JPEG.cpp
        JPEGLoad.cpp
        JPEGPlugin.cpp
        JPEGSave.cpp)
endif()
if(PNG_FOUND)
    set(header
        ${header}
        PNG.h
        PNGLoad.h
        PNGPlugin.h
        PNGSave.h)
    set(source
        ${source}
        PNG.cpp
        PNGLoad.cpp
        PNGPlugin.cpp
        PNGSave.cpp)
endif()
if(TIFF_FOUND)
    set(header
        ${header}
        TIFF.h
        TIFFLoad.h
        TIFFPlugin.h
        TIFFSave.h)
    set(source
        ${source}
        TIFF.cpp
        TIFFLoad.cpp
        TIFFPlugin.cpp
        TIFFSave.cpp)
endif()
if(OPENEXR_FOUND)
    set(header
        ${header}
        OpenEXR.h
        OpenEXRLoad.h
        OpenEXRPlugin.h
        OpenEXRSave.h)
    set(source
        ${source}
        OpenEXR.cpp
        OpenEXRLoad.cpp
        OpenEXRPlugin.cpp
        OpenEXRSave.cpp)
endif()
if(FFMPEG_FOUND)
    set(header
        ${header}
        FFmpeg.h
        FFmpegLoad.h
        FFmpegPlugin.h
        FFmpegSave.h)
    set(source
        ${source}
        FFmpeg.cpp
        FFmpegLoad.cpp
        FFmpegPlugin.cpp
        FFmpegSave.cpp)
endif()

QT5_WRAP_CPP(mocSource ${mocHeader})
QT5_ADD_RESOURCES(rccSource djvGraphics.qrc)
QT5_CREATE_TRANSLATION(qmSource ${source}
    djvGraphics_en_US.ts
    djvGraphics_cs_CZ.ts)

include_directories(${OPENGL_INCLUDE_DIRS})
add_library(djvGraphics ${header} ${source} ${mocSource} ${rccSource} ${qmSource})
set(djvGraphicsLibs
    djvCore
    IlmBase
    ${OPENGL_LIBRARIES})
if(JPEG_FOUND)
    set(djvGraphicsLibs ${djvGraphicsLibs} JPEG)
endif()
if(PNG_FOUND)
    set(djvGraphicsLibs ${djvGraphicsLibs} PNG)
endif()
if(TIFF_FOUND)
    set(djvGraphicsLibs ${djvGraphicsLibs} TIFF)
endif()
if(OPENEXR_FOUND)
    set(djvGraphicsLibs ${djvGraphicsLibs} OpenEXR)
endif()
if(FFMPEG_FOUND)
    set(djvGraphicsLibs ${djvGraphicsLibs} FFMPEG)
endif()
target_link_libraries(djvGraphics ${djvGraphicsLibs})
set_target_properties(djvGraphics PROPERTIES FOLDER lib CXX_STANDARD 11)

install(
    FILES ${qmSource}
    DESTINATION ${DJV_INSTALL_TRANSLATIONS})
if(BUILD_SHARED_LIBS)
    install(
        TARGETS djvGraphics
        RUNTIME DESTINATION ${DJV_INSTALL_BIN}
        LIBRARY DESTINATION ${DJV_INSTALL_LIB}
        ARCHIVE DESTINATION ${DJV_INSTALL_LIB})
endif()

