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

#include <djvUI/Core.h>

#include <djvGraphics/Color.h>

#include <QPointer>
#include <QWidget>

namespace djv
{
    namespace UI
    {
        class UIContext;

        //! This class provides a basic color swatch widget.
        class ColorSwatch : public QWidget
        {
            Q_OBJECT

            //! This property holds the color.    
            Q_PROPERTY(
                djv::Graphics::Color color
                READ                 color
                WRITE                setColor
                NOTIFY               colorChanged)

        public:
            explicit ColorSwatch(const QPointer<UIContext> &, QWidget * parent = nullptr);
            ~ColorSwatch() override;

            //! Get the color.
            const Graphics::Color & color() const;

            //! This enumeration provides the swatch size.
            enum SWATCH_SIZE
            {
                SWATCH_SMALL,
                SWATCH_MEDIUM,
                SWATCH_LARGE,
            };
            Q_ENUM(SWATCH_SIZE);

            //! Get the size.    
            SWATCH_SIZE swatchSize() const;

            //! Set the swatch size.
            void setSwatchSize(SWATCH_SIZE);

            //! Get whether the color dialog is enabled.
            bool isColorDialogEnabled() const;

            //! Set whether the color dialog is enabled.
            void setColorDialogEnabled(bool);

            QSize sizeHint() const override;

        public Q_SLOTS:
            //! Set the color.
            void setColor(const djv::Graphics::Color &);

        Q_SIGNALS:
            //! This signal is emitted when the color is changed.
            void colorChanged(const djv::Graphics::Color &);

            //! This signal is emitted when the swatch is clicked.
            void clicked();

        protected:
            void mousePressEvent(QMouseEvent *) override;
            void paintEvent(QPaintEvent *) override;
            bool event(QEvent*) override;

        private Q_SLOTS:
            void styleUpdate();

        private:
            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace UI
} // namespace djv
