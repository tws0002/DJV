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

#include <djvUI/FileBrowserModel.h>
#include <djvUI/Shortcut.h>

#include <QObject>

namespace djv
{
    namespace UI
    {
        class UIContext;

        //! This class provides file browser preferences.
        class FileBrowserPrefs : public QObject
        {
            Q_OBJECT

            //! This property holds whether hidden files are shown.
            Q_PROPERTY(
                bool   showHidden
                READ   hasShowHidden
                WRITE  setShowHidden
                NOTIFY showHiddenChanged)

            //! This property holds the columnsSorting.
            Q_PROPERTY(
                FileBrowserModel::COLUMNS columnsSort
                READ                      columnsSort
                WRITE                     setColumnsSort
                NOTIFY                    columnsSortChanged)

            //! This property holds whether sorting is reversed.
            Q_PROPERTY(
                bool   reverseSort
                READ   hasReverseSort
                WRITE  setReverseSort
                NOTIFY reverseSortChanged)

            //! This property holds whether directories are sorted first.
            Q_PROPERTY(
                bool   sortDirsFirst
                READ   hasSortDirsFirst
                WRITE  setSortDirsFirst
                NOTIFY sortDirsFirstChanged)

            //! This property holds the image thumbnail mode.
            Q_PROPERTY(
                FileBrowserModel::THUMBNAIL_MODE thumbnailMode
                READ                             thumbnailMode
                WRITE                            setThumbnailMode
                NOTIFY                           thumbnailModeChanged)

            //! This property holds the image thumbnail size.
            Q_PROPERTY(
                FileBrowserModel::THUMBNAIL_SIZE thumbnailSize
                READ                             thumbnailSize
                WRITE                            setThumbnailSize
                NOTIFY                           thumbnailSizeChanged)

            //! This property holds the image thumbnail cache size.
            Q_PROPERTY(
                qint64 thumbnailCache
                READ   thumbnailCache
                WRITE  setThumbnailCache
                NOTIFY thumbnailCacheChanged)

            //! This property holds the list of recent directories.
            Q_PROPERTY(
                QStringList recent
                READ        recent
                NOTIFY      recentChanged);

            //! This property holds the list of bookmarks.
            Q_PROPERTY(
                QStringList bookmarks
                READ        bookmarks
                WRITE       setBookmarks
                NOTIFY      bookmarksChanged);

            //! This property holds the list of shortcuts.
            Q_PROPERTY(
                QVector<Shortcut> shortcuts
                READ              shortcuts
                WRITE             setShortcuts
                NOTIFY            shortcutsChanged)

        public:
            explicit FileBrowserPrefs(const QPointer<UIContext> &, QObject * parent = nullptr);
            ~FileBrowserPrefs();

            //! Get the file system path default.
            static const QString & pathDefault();

            //! Get the hidden files shown default.
            static bool showHiddenDefault();

            //! Get whether hidden files are shown.
            bool hasShowHidden() const;

            //! Get the sorting default.
            static FileBrowserModel::COLUMNS columnsSortDefault();

            //! Get the sorting.
            FileBrowserModel::COLUMNS columnsSort() const;

            //! Get the the sorting reversed default.
            static bool reverseSortDefault();

            //! Get whether sorting is reversed.
            bool hasReverseSort() const;

            //! Get the directories sorted first default.
            static bool sortDirsFirstDefault();

            //! Get whether directories are sorted first.
            bool hasSortDirsFirst() const;

            //! Get the image thumbnail mode default.
            static FileBrowserModel::THUMBNAIL_MODE thumbnailModeDefault();

            //! Get the image thumbnail mode.
            FileBrowserModel::THUMBNAIL_MODE thumbnailMode() const;

            //! Get the image thumbnail size default.
            static FileBrowserModel::THUMBNAIL_SIZE thumbnailSizeDefault();

            //! Get the image thumbnail size.
            FileBrowserModel::THUMBNAIL_SIZE thumbnailSize() const;

            //! Get the image thumbnail cache size default.
            static qint64 thumbnailCacheDefault();

            //! Get the image thumbnail cache size.
            qint64 thumbnailCache() const;

            //! Get the list of recent directories.
            const QStringList & recent() const;

            //! Get the list of bookmarks.
            const QStringList & bookmarks() const;

            //! This enumeration provides the keyboard shortcuts.
            enum SHORTCUT
            {
                UP,
                PREV,
                CURRENT,
                HOME,
                DESKTOP,
                ROOT,
                RELOAD,
                SEQ_OFF,
                SEQ_SPARSE,
                SEQ_RANGE,
                SHOW_HIDDEN,
                SORT_NAME,
                SORT_SIZE,
#if ! defined(DJV_WINDOWS)
                SORT_USER,
#endif
                SORT_PERMISSIONS,
                SORT_TIME,
                REVERSE_SORT,
                SORT_DIRS_FIRST,
                ADD_BOOKMARK,
                BOOKMARK_1,
                BOOKMARK_2,
                BOOKMARK_3,
                BOOKMARK_4,
                BOOKMARK_5,
                BOOKMARK_6,
                BOOKMARK_7,
                BOOKMARK_8,
                BOOKMARK_9,
                BOOKMARK_10,
                BOOKMARK_11,
                BOOKMARK_12,

                SHORTCUT_COUNT
            };
            Q_ENUM(SHORTCUT);

            //! Get the shortcut labels.
            static const QStringList & shortcutLabels();

            //! Get the shortcut defaults.
            static const QVector<Shortcut> & shortcutsDefault();

            //! Get the shortcuts.
            const QVector<Shortcut> & shortcuts() const;

        public Q_SLOTS:
            //! Set the file system path default.
            static void setPathDefault(const QString &);

            //! Set whether hidden files are shown.
            void setShowHidden(bool);

            //! Set the sorting.
            void setColumnsSort(djv::UI::FileBrowserModel::COLUMNS);

            //! Set whether sorting is reversed.
            void setReverseSort(bool);

            //! Set whether directories are sorted first.
            void setSortDirsFirst(bool);

            //! Set the image thumbnail mode.
            void setThumbnailMode(djv::UI::FileBrowserModel::THUMBNAIL_MODE);

            //! Set the image thumbnail size.
            void setThumbnailSize(djv::UI::FileBrowserModel::THUMBNAIL_SIZE);

            //! Set the image thumbnail cache size.
            void setThumbnailCache(qint64);

            //! Set the list of recent directories.
            void setRecent(const QStringList &);

            //! Add a recent directory.
            void addRecent(const QString &);

            //! Clear the list of recent directories.
            void clearRecent();

            //! Set the list of bookmarks.
            void setBookmarks(const QStringList &);

            //! Add a bookmark.
            void addBookmark(const QString &);

            //! Set the list of shortcuts.
            void setShortcuts(const QVector<Shortcut> &);

        Q_SIGNALS:
            //! This signal is emitted when the hidden files are changed.
            void showHiddenChanged(bool);

            //! This signal is emitted when the sorting is changed.
            void columnsSortChanged(djv::UI::FileBrowserModel::COLUMNS);

            //! This signal is emitted when reverse sorting is changed.
            void reverseSortChanged(bool);

            //! This signal is emitted when sort directories first is changed.
            void sortDirsFirstChanged(bool);

            //! This signal is emitted when the image thumbnail mode is changed.
            void thumbnailModeChanged(djv::UI::FileBrowserModel::THUMBNAIL_MODE);

            //! This signal is emitted when the image thumbnail size is changed.
            void thumbnailSizeChanged(djv::UI::FileBrowserModel::THUMBNAIL_SIZE);

            //! This signal is emitted when the image thumbnail cache size is changed.
            void thumbnailCacheChanged(qint64);

            //! This signal is emitted when the recent directories are changed.
            void recentChanged(const QStringList &);

            //! This signal is emitted when the bookmarks are changed.
            void bookmarksChanged(const QStringList &);

            //! This signal is emitted when the list of shortcuts are changed.
            void shortcutsChanged(const QVector<djv::UI::Shortcut> &);

            //! This signal is emitted when a preference is changed.
            void prefChanged();

        private:
            DJV_PRIVATE_COPY(FileBrowserPrefs);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace UI
} // namespace djv

