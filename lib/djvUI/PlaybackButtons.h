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

#include <djvUI/PlaybackUtil.h>

#include <QWidget>

#include <memory>

class QButtonGroup;

namespace djv
{
    namespace UI
    {
        class UIContext;

        //! This class provides playback buttons.
        class PlaybackButtons : public QWidget
        {
            Q_OBJECT

            //! This property holds the current playback state.    
            Q_PROPERTY(
                PlaybackUtil::PLAYBACK playback
                READ                   playback
                WRITE                  setPlayback
                NOTIFY                 playbackChanged)

        public:
            explicit PlaybackButtons(const QPointer<UIContext> &, QWidget * parent = nullptr);
            ~PlaybackButtons() override;
            
            //! Get the current playback state.    
            PlaybackUtil::PLAYBACK playback() const;

        public Q_SLOTS:
            //! Set the current playback state.
            void setPlayback(PlaybackUtil::PLAYBACK);

        Q_SIGNALS:
            //! This signal is emitted when the current playback state is changed.
            void playbackChanged(PlaybackUtil::PLAYBACK);

        protected:
            bool event(QEvent*) override;

        private Q_SLOTS:
            void buttonCallback(int);

            void styleUpdate();

        private:
            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace UI
} // namespace djv

