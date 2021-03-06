//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
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

#include <djvGraphics/JPEGSave.h>

#include <djvGraphics/OpenGLImage.h>

#include <djvCore/CoreContext.h>
#include <djvCore/Error.h>
#include <djvCore/StringUtil.h>

namespace djv
{
    namespace Graphics
    {
        JPEGSave::JPEGSave(const JPEG::Options & options, const QPointer<Core::CoreContext> & context) :
            ImageSave(context),
            _options(options)
        {}

        JPEGSave::~JPEGSave()
        {
            close();
        }

        void JPEGSave::open(const Core::FileInfo & in, const ImageIOInfo & info)
        {
            //DJV_DEBUG("JPEGSave::open");
            //DJV_DEBUG_PRINT("in = " << in);
            _file = in;
            if (info.sequence.frames.count() > 1)
            {
                _file.setType(Core::FileInfo::SEQUENCE);
            }
            _info = ImageIOInfo();
            _info.size = info.size;
            Pixel::FORMAT format = Pixel::format(info.pixel);
            switch (format)
            {
            case Pixel::LA:
                format = Pixel::L;
                break;
            case Pixel::RGBA:
                format = Pixel::RGB;
                break;
            default: break;
            }
            _info.pixel = Pixel::pixel(format, Pixel::U8);
            //DJV_DEBUG_PRINT("info = " << _info);
            _image.set(_info);
        }

        namespace
        {

            bool jpegScanline(
                jpeg_compress_struct * jpeg,
                const quint8 *         in,
                JPEGErrorStruct *      error)
            {
                if (::setjmp(error->jump))
                {
                    return false;
                }
                JSAMPROW p[] = { (JSAMPLE *)(in) };
                if (!jpeg_write_scanlines(jpeg, p, 1))
                {
                    return false;
                }
                return true;
            }

            bool jpeg_end(jpeg_compress_struct * jpeg, JPEGErrorStruct * error)
            {
                if (::setjmp(error->jump))
                {
                    return false;
                }
                jpeg_finish_compress(jpeg);
                return true;
            }

        } // namespace

        void JPEGSave::write(const Image & in, const ImageIOFrameInfo & frame)
        {
            //DJV_DEBUG("JPEGSave::write");
            //DJV_DEBUG_PRINT("in = " << in);

            // Open the file.
            const QString fileName = _file.fileName(frame.frame);
            //DJV_DEBUG_PRINT("file name = " << fileName);
            ImageIOInfo info(_info);
            info.tags = in.tags;
            _open(fileName, info);

            // Convert the image.
            const PixelData * p = &in;
            if (in.info() != _info)
            {
                //DJV_DEBUG_PRINT("convert = " << _image);
                _image.zero();
                OpenGLImage().copy(in, _image);
                p = &_image;
            }

            // Write the file.
            const int h = p->h();
            for (int y = 0; y < h; ++y)
            {
                if (!jpegScanline(&_jpeg, p->data(0, h - 1 - y), &_jpegError))
                {
                    throw Core::Error(JPEG::staticName, _jpegError.msg);
                }
            }
            if (!jpeg_end(&_jpeg, &_jpegError))
            {
                throw Core::Error(JPEG::staticName, _jpegError.msg);
            }
            close();
        }

        void JPEGSave::close()
        {
            if (_jpegInit)
            {
                jpeg_destroy_compress(&_jpeg);
                _jpegInit = false;
            }
            if (_f)
            {
                ::fclose(_f);
                _f = 0;
            }
        }

        namespace
        {

            bool jpegInit(jpeg_compress_struct * jpeg, JPEGErrorStruct * error)
            {
                if (setjmp(error->jump))
                {
                    return false;
                }
                jpeg_create_compress(jpeg);
                return true;
            }

            bool jpegOpen(
                FILE *                 f,
                jpeg_compress_struct * jpeg,
                const ImageIOInfo &    info,
                int                    quality,
                JPEGErrorStruct *      error)
            {
                if (setjmp(error->jump))
                {
                    return false;
                }
                jpeg_stdio_dest(jpeg, f);
                jpeg->image_width = info.size.x;
                jpeg->image_height = info.size.y;
                if (Pixel::L_U8 == info.pixel)
                {
                    jpeg->input_components = 1;
                    jpeg->in_color_space = JCS_GRAYSCALE;
                }
                else if (Pixel::RGB_U8 == info.pixel)
                {
                    jpeg->input_components = 3;
                    jpeg->in_color_space = JCS_RGB;
                }
                jpeg_set_defaults(jpeg);
                jpeg_set_quality(jpeg, quality, static_cast<boolean>(1));
                jpeg_start_compress(jpeg, static_cast<boolean>(1));
                QString tag = info.tags[ImageTags::tagLabels()[ImageTags::DESCRIPTION]];
                if (tag.length())
                {
                    jpeg_write_marker(
                        jpeg,
                        JPEG_COM,
                        (JOCTET *)Core::StringUtil::qToStdWString(tag).data(),
                        static_cast<uint>(tag.length()));
                }
                return true;
            }

        } // namespace

        void JPEGSave::_open(const QString & in, const ImageIOInfo & info)
        {
            //DJV_DEBUG("JPEGSave::_open");
            //DJV_DEBUG_PRINT("in = " << in);

            close();

            // Initialize libjpeg.
            _jpeg.err = jpeg_std_error(&_jpegError.pub);
            _jpegError.pub.error_exit = djvJPEGError;
            _jpegError.pub.emit_message = djvJPEGWarning;
            _jpegError.msg[0] = 0;
            if (!jpegInit(&_jpeg, &_jpegError))
            {
                throw Core::Error(JPEG::staticName, _jpegError.msg);
            }
            _jpegInit = true;

            // Open the file.
#if defined(DJV_WINDOWS)
            ::_wfopen_s(&_f, Core::StringUtil::qToStdWString(in).data(), L"wb");
#else // DJV_WINDOWS
            _f = ::fopen(in.toUtf8().data(), "wb");
#endif // DJV_WINDOWS
            if (!_f)
            {
                throw Core::Error(
                    JPEG::staticName,
                    ImageIO::errorLabels()[ImageIO::ERROR_OPEN]);
            }
            if (!jpegOpen(_f, &_jpeg, info, _options.quality, &_jpegError))
            {
                throw Core::Error(JPEG::staticName, _jpegError.msg);
            }
        }

    } // namespace Graphics
} // namespace djv
