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

#include <djvViewLib/FileExport.h>

#include <djvViewLib/Util.h>
#include <djvViewLib/ViewContext.h>

#include <djvUI/ProgressDialog.h>

#include <djvGraphics/Image.h>
#include <djvGraphics/PixelDataUtil.h>

#include <djvCore/Error.h>

#include <QApplication>
#include <QDir>

namespace djv
{
    namespace ViewLib
    {
        FileExportInfo::FileExportInfo(
            const Core::FileInfo &               inputFile,
            const Core::FileInfo &               outputFile,
            const Graphics::PixelDataInfo &      info,
            const Core::Sequence &               sequence,
            int                                  layer,
            Graphics::PixelDataInfo::PROXY       proxy,
            bool                                 u8Conversion,
            bool                                 colorProfile,
            const Graphics::OpenGLImageOptions & options) :
            inputFile(inputFile),
            outputFile(outputFile),
            info(info),
            sequence(sequence),
            layer(layer),
            proxy(proxy),
            u8Conversion(u8Conversion),
            colorProfile(colorProfile),
            options(options)
        {}

        struct FileExport::Private
        {
            Private(const QPointer<ViewContext> & context) :
                context(context)
            {}

            FileExportInfo info;
            Core::Sequence saveSequence;
            QScopedPointer<Graphics::ImageLoad> load;
            QScopedPointer<Graphics::ImageSave> save;
            std::unique_ptr<Graphics::OpenGLImage> openGLImage;
            QPointer<UI::ProgressDialog> dialog;
            QPointer<ViewContext> context;
        };

        FileExport::FileExport(const QPointer<ViewContext> & context, QObject * parent) :
            QObject(parent),
            _p(new Private(context))
        {
            _p->dialog = new UI::ProgressDialog;

            connect(
                _p->dialog,
                SIGNAL(progressSignal(int)),
                SLOT(callback(int)));
            connect(
                _p->dialog,
                SIGNAL(finishedSignal()),
                SLOT(finishedCallback()));
        }

        FileExport::~FileExport()
        {
            //DJV_DEBUG("FileExport::~FileExport");
            delete _p->dialog;

            _p->context->makeGLContextCurrent();
            _p->openGLImage.reset();
        }

        void FileExport::start(const FileExportInfo & info)
        {
            //DJV_DEBUG("FileExport::start");
            //DJV_DEBUG_PRINT("input = " << info.inputFile);
            //DJV_DEBUG_PRINT("output = " << info.outputFile);
            //DJV_DEBUG_PRINT("sequence = " << info.sequence);

            cancel();

            _p->info = info;
            if (_p->info.outputFile.isSequenceValid())
            {
                _p->saveSequence = Core::Sequence(
                    _p->info.outputFile.sequence().frames[0],
                    _p->info.outputFile.sequence().frames[0] + info.sequence.frames.count() - 1,
                    info.sequence.pad,
                    info.sequence.speed);
            }
            else
            {
                _p->saveSequence = info.sequence;
            }
            
            //DJV_DEBUG_PRINT("save sequence = " << _p->saveSequence);

            //! \todo Why do we need to reverse the rotation here?
            _p->info.options.xform.rotate = -_p->info.options.xform.rotate;
            const Core::Box2f bbox =
                glm::mat3x3(Graphics::OpenGLImageXform::xformMatrix(_p->info.options.xform)) *
                Core::Box2f(_p->info.info.size * Graphics::PixelDataUtil::proxyScale(_p->info.info.proxy));
            //DJV_DEBUG_PRINT("bbox = " << bbox);
            _p->info.options.xform.position = -bbox.position;
            _p->info.info.size = bbox.size;

            // Open input.
            Graphics::ImageIOInfo loadInfo;
            try
            {
                _p->load.reset(_p->context->imageIOFactory()->load(_p->info.inputFile, loadInfo));
            }
            catch (Core::Error error)
            {
                error.add(
                    Enum::errorLabels()[Enum::ERROR_OPEN_IMAGE].
                    arg(QDir::toNativeSeparators(_p->info.inputFile)));
                _p->context->printError(error);
                return;
            }

            // Open output.
            Graphics::ImageIOInfo saveInfo(_p->info.info);
            saveInfo.tags = loadInfo.tags;
            saveInfo.sequence = _p->saveSequence;
            try
            {
                _p->save.reset(_p->context->imageIOFactory()->save(_p->info.outputFile, saveInfo));
            }
            catch (Core::Error error)
            {
                error.add(
                    Enum::errorLabels()[Enum::ERROR_OPEN_IMAGE].
                    arg(QDir::toNativeSeparators(_p->info.outputFile)));
                _p->context->printError(error);
                return;
            }

            // Start...
            _p->dialog->setLabel(qApp->translate("djv::ViewLib::FileExport", "Exporting \"%1\":").
                arg(QDir::toNativeSeparators(_p->info.outputFile)));
            _p->dialog->start(_p->info.sequence.frames.count() ? _p->info.sequence.frames.count() : 1);
            _p->dialog->show();
        }

        void FileExport::cancel()
        {
            //DJV_DEBUG("FileExport::cancel");
            if (_p->dialog->isVisible())
            {
                _p->dialog->reject();
            }
            if (_p->save.data())
            {
                try
                {
                    //DJV_DEBUG_PRINT("close");

                    _p->save->close();
                }
                catch (Core::Error error)
                {
                    error.add(
                        Enum::errorLabels()[Enum::ERROR_WRITE_IMAGE].
                        arg(QDir::toNativeSeparators(_p->info.outputFile)));
                    _p->context->printError(error);
                }
            }
            _p->info = FileExportInfo();
            _p->load.reset();
            _p->save.reset();
        }

        void FileExport::callback(int in)
        {
            //DJV_DEBUG("FileExport::callback");
            //DJV_DEBUG_PRINT("in = " << in);

            _p->context->makeGLContextCurrent();
            if (!_p->openGLImage)
            {
                _p->openGLImage.reset(new Graphics::OpenGLImage);
            }

            // Load the frame.
            Graphics::Image image;
            try
            {
                const qint64 frame = in < _p->info.sequence.frames.count() ? _p->info.sequence.frames[in] : -1;
                //DJV_DEBUG_PRINT("frame = " << frame);
                _p->load->read(image, Graphics::ImageIOFrameInfo(frame, _p->info.layer, _p->info.proxy));
                //DJV_DEBUG_PRINT("image = " << image);
            }
            catch (Core::Error error)
            {
                error.add(
                    Enum::errorLabels()[Enum::ERROR_READ_IMAGE].
                    arg(QDir::toNativeSeparators(_p->info.inputFile)));
                _p->context->printError(error);
                cancel();
                return;
            }

            // Process the frame.
            Graphics::Image * p = &image;
            Graphics::Image tmp;
            Graphics::OpenGLImageOptions options(_p->info.options);
            if (_p->info.u8Conversion || _p->info.colorProfile)
            {
                options.colorProfile = image.colorProfile;
            }
            //DJV_DEBUG_PRINT("convert = " << (p->info() != _p->info.info));
            //DJV_DEBUG_PRINT("options = " << (options != Graphics::OpenGLImageOptions()));
            //DJV_DEBUG_PRINT("options = " << options);
            //DJV_DEBUG_PRINT("def options = " << Graphics::OpenGLImageOptions());
            if (p->info() != _p->info.info || options != Graphics::OpenGLImageOptions())
            {
                tmp.set(_p->info.info);
                try
                {
                    //DJV_DEBUG_PRINT("process");
                    _p->openGLImage->copy(image, tmp, options);
                }
                catch (Core::Error error)
                {
                    error.add(
                        Enum::errorLabels()[Enum::ERROR_WRITE_IMAGE].
                        arg(QDir::toNativeSeparators(_p->info.outputFile)));
                    _p->context->printError(error);
                    cancel();
                    return;
                }
                p = &tmp;
            }
            tmp.tags = image.tags;

            // Save the frame.
            try
            {
                //DJV_DEBUG_PRINT("save");
                _p->save->write(
                    tmp,
                    Graphics::ImageIOFrameInfo(
                        in < _p->saveSequence.frames.count() ? _p->saveSequence.frames[in] : -1));
            }
            catch (Core::Error error)
            {
                error.add(
                    Enum::errorLabels()[Enum::ERROR_WRITE_IMAGE].
                    arg(QDir::toNativeSeparators(_p->info.outputFile)));
                _p->context->printError(error);
                cancel();
                return;
            }
            if ((_p->saveSequence.frames.count() - 1) == in)
            {
                try
                {
                    //DJV_DEBUG_PRINT("close");
                    _p->save->close();
                }
                catch (Core::Error error)
                {
                    error.add(
                        Enum::errorLabels()[Enum::ERROR_WRITE_IMAGE].
                        arg(QDir::toNativeSeparators(_p->info.outputFile)));
                    _p->context->printError(error);
                }
            }
        }

        void FileExport::finishedCallback()
        {
            //DJV_DEBUG("FileExport::finishedCallback");
            try
            {
                //DJV_DEBUG_PRINT("close");
                _p->save->close();
            }
            catch (Core::Error error)
            {
                error.add(
                    Enum::errorLabels()[Enum::ERROR_WRITE_IMAGE].
                    arg(QDir::toNativeSeparators(_p->info.outputFile)));
                _p->context->printError(error);
            }
            Q_EMIT finished();
        }

    } // namespace ViewLib
} // namespace djv
