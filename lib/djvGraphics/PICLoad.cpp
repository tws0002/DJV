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

#include <djvGraphics/PICLoad.h>

#include <djvGraphics/Image.h>
#include <djvGraphics/PixelDataUtil.h>

#include <djvCore/CoreContext.h>
#include <djvCore/FileIO.h>

namespace djv
{
    namespace Graphics
    {
        PICLoad::PICLoad(const QPointer<Core::CoreContext> & context) :
            ImageLoad(context)
        {}

        PICLoad::~PICLoad()
        {}

        void PICLoad::open(const Core::FileInfo & in, ImageIOInfo & info)
        {
            //DJV_DEBUG("PICLoad::open");
            //DJV_DEBUG_PRINT("in = " << in);
            _file = in;
            Core::FileIO io;
            _open(_file.fileName(_file.sequence().start()), info, io);
            if (Core::FileInfo::SEQUENCE == _file.type())
            {
                info.sequence.frames = _file.sequence().frames;
            }
        }

        void PICLoad::read(Image & image, const ImageIOFrameInfo & frame)
        {
            //DJV_DEBUG("PICLoad::read");
            //DJV_DEBUG_PRINT("frame = " << frame);

            image.colorProfile = ColorProfile();
            image.tags = ImageTags();

            // Open the file.
            const QString fileName =
                _file.fileName(frame.frame != -1 ? frame.frame : _file.sequence().start());
            //DJV_DEBUG_PRINT("file name = " << fileName);
            ImageIOInfo info;
            Core::FileIO io;
            _open(fileName, info, io);
            image.tags = info.tags;

            // Read the file.
            io.readAhead();
            PixelData * data = frame.proxy ? &_tmp : &image;
            data->set(info);
            const int  channels = Pixel::channels(info.pixel);
            const int  byteCount = Pixel::channelByteCount(info.pixel);
            const bool endian = io.endian();
            const quint8 * p = io.mmapP(), *const end = io.mmapEnd();
            for (int y = 0; y < info.size.y; ++y)
            {
                //DJV_DEBUG_PRINT("y = " << y);
                switch (_type)
                {
                case PIC::TYPE_RGB:
                case PIC::TYPE_RGBA:
                    if (_compression[0])
                    {
                        p = PIC::readRle(
                            p,
                            end,
                            data->data(0, y),
                            info.size.x,
                            channels,
                            channels * byteCount,
                            endian);
                        if (!p)
                        {
                            throw Core::Error(
                                PIC::staticName,
                                ImageIO::errorLabels()[ImageIO::ERROR_READ]);
                        }
                    }
                    else
                    {
                        const int size = info.size.x * channels * byteCount;
                        if ((io.size() - io.pos()) <
                            PixelDataUtil::dataByteCount(info))
                        {
                            throw Core::Error(
                                PIC::staticName,
                                ImageIO::errorLabels()[ImageIO::ERROR_READ]);
                        }

                        memcpy(data->data(0, y), p, size);
                        p += size;
                    }
                    break;
                case PIC::TYPE_RGB_A:

                    if (_compression[0])
                    {
                        p = PIC::readRle(
                            p,
                            end,
                            data->data(0, y),
                            info.size.x,
                            3,
                            channels * byteCount,
                            endian);
                        if (!p)
                        {
                            throw Core::Error(
                                PIC::staticName,
                                ImageIO::errorLabels()[ImageIO::ERROR_READ]);
                        }
                    }
                    else
                    {
                        const int size = info.size.x * 3 * byteCount;
                        if ((io.size() - io.pos()) <
                            PixelDataUtil::dataByteCount(info))
                        {
                            throw Core::Error(
                                PIC::staticName,
                                ImageIO::errorLabels()[ImageIO::ERROR_READ]);
                        }
                        memcpy(data->data(0, y), p, size);
                        p += size;
                    }
                    if (_compression[1])
                    {
                        p = PIC::readRle(
                            p,
                            end,
                            data->data(0, y) + 3 * byteCount,
                            info.size.x,
                            1,
                            channels * byteCount,
                            endian);
                        if (!p)
                        {
                            throw Core::Error(
                                PIC::staticName,
                                ImageIO::errorLabels()[ImageIO::ERROR_READ]);
                        }
                    }
                    else
                    {
                        const int size = info.size.x * 1 * byteCount;
                        if ((io.size() - io.pos()) <
                            PixelDataUtil::dataByteCount(info))
                        {
                            throw Core::Error(
                                PIC::staticName,
                                ImageIO::errorLabels()[ImageIO::ERROR_READ]);
                        }
                        memcpy(data->data(0, y), p, size);
                        p += size;
                    }
                    break;

                default: break;
                }
            }

            // Proxy scale the image.
            if (frame.proxy)
            {
                info.size = PixelDataUtil::proxyScale(info.size, frame.proxy);
                info.proxy = frame.proxy;
                image.set(info);
                PixelDataUtil::proxyScale(_tmp, image, frame.proxy);
            }

            //DJV_DEBUG_PRINT("image = " << image);
        }

        namespace
        {
            struct Header
            {
                quint32 magic;
                float   version;
                char    comment[80];
                char    id[4];
                quint16 width;
                quint16 height;
                float   ratio;
                quint16 fields;
                quint8  pad[2];
            };

            enum CHANNEL
            {
                CHANNEL_A = 0x10,
                CHANNEL_B = 0x20,
                CHANNEL_G = 0x40,
                CHANNEL_R = 0x80
            };

            struct Channel
            {
                quint8 chained;
                quint8 size;
                quint8 type;
                quint8 channel;
            };

            void _channel(Core::FileIO & io, Channel * out)
            {
                io.getU8(&out->chained);
                io.getU8(&out->size);
                io.getU8(&out->type);
                io.getU8(&out->channel);
            }

            /*String debugChannel(int in)
            {
                String out;
                if (in & CHANNEL_R) out += 'R';
                if (in & CHANNEL_G) out += 'G';
                if (in & CHANNEL_B) out += 'B';
                if (in & CHANNEL_A) out += 'A';
                return out;
            }*/

        } /// namespace

        void PICLoad::_open(const QString & in, ImageIOInfo & info, Core::FileIO & io)
        {
            //DJV_DEBUG("PICLoad::_open");
            //DJV_DEBUG_PRINT("in = " << in);

            // Open the file.
            io.setEndian(Core::Memory::endian() != Core::Memory::MSB);
            io.open(in, Core::FileIO::READ);

            // Read the header.
            Header header;
            memset(&header, 0, sizeof(Header));
            io.getU32(&header.magic);
            if (header.magic != 0x5380F634)
            {
                throw Core::Error(
                    PIC::staticName,
                    ImageIO::errorLabels()[ImageIO::ERROR_UNSUPPORTED]);
            }
            io.getF32(&header.version);
            io.get(header.comment, sizeof(header.comment));
            io.get(header.id, sizeof(header.id));
            io.getU16(&header.width);
            io.getU16(&header.height);
            io.getF32(&header.ratio);
            io.getU16(&header.fields);
            io.seek(sizeof(header.pad));
            //DJV_DEBUG_PRINT("version = " << header.version);
            //DJV_DEBUG_PRINT("comment = " << String(header.comment, sizeof(header.comment)));
            //DJV_DEBUG_PRINT("id = " << String(header.id, sizeof(header.id)));
            //DJV_DEBUG_PRINT("width = " << header.width);
            //DJV_DEBUG_PRINT("height = " << header.height);
            //DJV_DEBUG_PRINT("ratio = " << header.ratio);
            //DJV_DEBUG_PRINT("fields = " << header.fields);
            if (QString::fromLatin1(header.id, sizeof(header.id)) != "PICT")
            {
                throw Core::Error(
                    PIC::staticName,
                    ImageIO::errorLabels()[ImageIO::ERROR_UNSUPPORTED]);
            }

            // Information.
            info.fileName = in;
            info.size = glm::ivec2(header.width, header.height);
            info.mirror.y = true;
            Channel channel;
            memset(&channel, 0, sizeof(Channel));
            _channel(io, &channel);
            _compression[0] = 2 == channel.type;
            //DJV_DEBUG_PRINT("channel = " << debugChannel(channel.channel));
            int type = -1;
            if ((CHANNEL_R & channel.channel) &&
                (CHANNEL_G & channel.channel) &&
                (CHANNEL_B & channel.channel) &&
                (CHANNEL_A & channel.channel) &&
                8 == channel.size &&
                !channel.chained)
            {
                type = PIC::TYPE_RGBA;
            }
            else if (
                (CHANNEL_R & channel.channel) &&
                (CHANNEL_G & channel.channel) &&
                (CHANNEL_B & channel.channel) &&
                8 == channel.size &&
                !channel.chained)
            {
                type = PIC::TYPE_RGB;
            }
            else if (
                (CHANNEL_R & channel.channel) &&
                (CHANNEL_G & channel.channel) &&
                (CHANNEL_B & channel.channel) &&
                8 == channel.size &&
                channel.chained)
            {
                memset(&channel, 0, sizeof(Channel));
                _channel(io, &channel);
                _compression[1] = 2 == channel.type;
                //DJV_DEBUG_PRINT("channel = " << debugChannel(channel.channel));
                if (!(CHANNEL_R & channel.channel) &&
                    !(CHANNEL_G & channel.channel) &&
                    !(CHANNEL_B & channel.channel) &&
                    (CHANNEL_A & channel.channel) &&
                    8 == channel.size &&
                    !channel.chained)
                {
                    type = PIC::TYPE_RGB_A;
                }
            }
            if (-1 == type)
            {
                throw Core::Error(
                    PIC::staticName,
                    ImageIO::errorLabels()[ImageIO::ERROR_UNSUPPORTED]);
            }
            _type = static_cast<PIC::TYPE>(type);
            switch (_type)
            {
            case PIC::TYPE_RGB:
                info.pixel = Pixel::RGB_U8;
                break;
            case PIC::TYPE_RGBA:
            case PIC::TYPE_RGB_A:
                info.pixel = Pixel::RGBA_U8;
                break;
            default: break;
            }

            // Read image tags.
            if (header.comment[0])
                info.tags[ImageTags::tagLabels()[ImageTags::DESCRIPTION]] =
                QString::fromLatin1(header.comment, sizeof(header.comment));

            //DJV_DEBUG_PRINT("info = " << info);
        }

    } // namespace Graphics
} // namespace djv
