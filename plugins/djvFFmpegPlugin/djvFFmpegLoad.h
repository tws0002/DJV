//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

//! \file djvFFmpegLoad.h

#ifndef DJV_FFMPEG_LOAD_H
#define DJV_FFMPEG_LOAD_H

#include <djvFFmpegPlugin.h>

#include <djvFileInfo.h>

//! \addtogroup djvFFmpegPlugin
//@{

//------------------------------------------------------------------------------
//! \class djvFFmpegLoad
//!
//! This class provides a FFmpeg loader.
//------------------------------------------------------------------------------

class djvFFmpegLoad : public djvImageLoad
{
public:

    //! Constructor.
    
    djvFFmpegLoad();

    //! Destructor.
    
    virtual ~djvFFmpegLoad();

    virtual void open(const djvFileInfo &, djvImageIoInfo &)
        throw (djvError);

    virtual void read(djvImage &, const djvImageIoFrameInfo &)
        throw (djvError);

    virtual void close() throw (djvError);

private:

    int readFrame(int64_t & pts);

    djvImageIoInfo    _info;
    
    AVFormatContext * _avFormatContext;
    int               _avVideoStream;
    AVCodecContext *  _avCodecContext;
    AVFrame *         _avFrame;
    AVFrame *         _avFrameRgb;
    SwsContext *      _swsContext;

    int               _startFrame;
    int               _frame;
    djvPixelData      _tmp;
};

//@} // djvFFmpegPlugin

#endif // DJV_FFMPEG_LOAD_H
