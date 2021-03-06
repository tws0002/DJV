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

#pragma once

#include <djvGraphics/ImageIO.h>
#include <djvGraphics/OpenEXR.h>

#include <djvCore/FileInfo.h>

#include <ImfInputFile.h>

namespace djv
{
    namespace Graphics
    {
        //! This class provides an OpenEXR loader.
        class OpenEXRLoad : public ImageLoad
        {
        public:
            OpenEXRLoad(const OpenEXR::Options &, const QPointer<Core::CoreContext> &);
            ~OpenEXRLoad() override;

            void open(const Core::FileInfo &, ImageIOInfo &) override;
            void read(Image &, const ImageIOFrameInfo &) override;
            void close() override;

        private:
            void _open(const QString &, ImageIOInfo &);

            OpenEXR::Options        _options;
            Core::FileInfo          _file;
            Imf::InputFile *        _f = nullptr;
            Core::Box2i             _displayWindow;
            Core::Box2i             _dataWindow;
            Core::Box2i             _intersectedWindow;
            QVector<OpenEXR::Layer> _layers;
            PixelData               _tmp;
            bool                    _fast = false;
        };

    } // namespace Graphics
} // namespace djv
