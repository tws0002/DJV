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

#include <djvCore/FileInfo.h>
#include <djvCore/Util.h>

#include <QDialog>

#include <memory>

class QModelIndex;

namespace djv
{
    namespace UI
    {
        class FileBrowserModel;
        class UIContext;

        //! This class provides a file browser dialog. The file browser dialog is used
        //! to load or save files.
        class FileBrowser : public QDialog
        {
            Q_OBJECT

            //! This property holds the file information.    
            Q_PROPERTY(
                djv::Core::FileInfo fileInfo
                READ                fileInfo
                WRITE               setFileInfo
                NOTIFY              fileInfoChanged)

            //! This property holds whether the file browser is pinnable.
            Q_PROPERTY(
                bool   pinnable
                READ   isPinnable
                WRITE  setPinnable
                NOTIFY pinnableChanged)

            //! This property holds whether the file browser is currently pinned.
            Q_PROPERTY(
                bool   pinned
                READ   isPinned
                WRITE  setPinned
                NOTIFY pinnedChanged)

        public:
            explicit FileBrowser(const QPointer<UIContext> &, QWidget * parent = nullptr);
            ~FileBrowser() override;

            //! Get the file information.
            const Core::FileInfo & fileInfo() const;

            //! Get whether the file browser is pinnable.
            bool isPinnable() const;

            //! Get whether the file browser is currently pinned.
            bool isPinned() const;
            
        public Q_SLOTS:
            //! Set the file information.
            void setFileInfo(const djv::Core::FileInfo &);

            //! Set whether the file browser is pinnable.
            void setPinnable(bool);

            //! Set whether the file browser is currently pinned.
            void setPinned(bool);

        Q_SIGNALS:
            //! This signal is emitted when the file information is changed.
            void fileInfoChanged(const djv::Core::FileInfo &);

            //! This signal is emitted when the pinnable state is changed.
            void pinnableChanged(bool);

            //! This signal is emitted when the pinned state is changed.
            void pinnedChanged(bool);

        protected:
            void showEvent(QShowEvent *) override;

        private Q_SLOTS:
            void fileCallback();
            void browserCallback(const QModelIndex &);
            void browserCurrentCallback(const QModelIndex &, const QModelIndex &);
            void upCallback();
            void prevCallback();
            void currentCallback();
            void recentCallback(QAction *);
            void drivesCallback(QAction  *);
            void homeCallback();
            void desktopCallback();
            void reloadCallback();
            void thumbnailModeCallback(QAction *);
            void thumbnailSizeCallback(QAction *);
            void showHiddenCallback(bool);
            void showHiddenCallback();
            void seqCallback(QAction *);
            void seqCallback(int);
            void searchCallback(const QString &);
            void columnsSortCallback(int, Qt::SortOrder);
            void columnsSortCallback(QAction *);
            void columnsSortCallback();
            void reverseSortCallback(bool);
            void reverseSortCallback();
            void sortDirsFirstCallback(bool);
            void sortDirsFirstCallback();
            void pinnedCallback(bool);
            void addBookmarkCallback();
            void deleteBookmarkCallback();
            void deleteAllBookmarksCallback();
            void bookmarksCallback(QAction *);
            void bookmarksCallback();
            void bookmarkDelCallback(const QVector<int> &);
            void bookmarkDelAllCallback();
            void acceptedCallback();
            
            void styleUpdate();
            void modelUpdate();
            void widgetUpdate();
            void menuUpdate();
            void toolTipUpdate();

        protected:
            void keyPressEvent(QKeyEvent *) override;
            bool event(QEvent *) override;
            
        private:
            QVector<int> columnSizes() const;

            DJV_PRIVATE_COPY(FileBrowser);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace UI
} // namespace djv
