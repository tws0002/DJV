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

//#undef DJV_MMAP

#include <djvCore/FileIO.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvCore/Error.h>
#include <djvCore/ErrorUtil.h>
#include <djvCore/FileInfo.h>
#include <djvCore/Math.h>
#include <djvCore/Memory.h>

#include <QCoreApplication>
#include <QDir>

#if defined(DJV_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif // DJV_WINDOWS
#if defined(DJV_MMAP)
#if ! defined(DJV_WINDOWS)
#include <sys/mman.h>
#endif // ! DJV_WINDOWS
#endif // DJV_MMAP
#if defined(DJV_WINDOWS)
#include <io.h>
#else // DJV_WINDOWS
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#endif // DJV_WINDOWS

#include <errno.h>
#include <stdio.h>

namespace djv
{
    namespace Core
    {
        struct FileIO::Private
        {
            Private() :
#if defined(DJV_WINDOWS)
                f(INVALID_HANDLE_VALUE)
            {}
#else // DJV_WINDOWS
                f(-1)
            {}

#endif // DJV_WINDOWS
#if defined(DJV_WINDOWS)
            HANDLE          f;
#else // DJV_WINDOWS
            int             f;
#endif // DJV_WINDOWS
            QString         fileName;
            FileIO::MODE    mode = static_cast<FileIO::MODE>(0);
            quint64         pos = 0;
            quint64         size = 0;
            bool            endian = false;
            void *          mmap = nullptr;
            const quint8 *  mmapStart = nullptr;
            const quint8 *  mmapEnd = nullptr;
            const quint8 *  mmapP = nullptr;
        };

        FileIO::FileIO() :
            _p(new Private)
        {}

        FileIO::~FileIO()
        {
            close();
        }

        void FileIO::open(const QString & fileName, MODE mode)
        {
            //DJV_DEBUG("FileIO::open");
            //DJV_DEBUG_PRINT("file name = " << fileName);
            //DJV_DEBUG_PRINT("mode = " << mode);

            close();

            // Open the file.
#if defined(DJV_WINDOWS)
            _p->f = ::CreateFileW(
                StringUtil::qToStdWString(fileName).data(),
                (WRITE == mode) ? GENERIC_WRITE : GENERIC_READ,
                (WRITE == mode) ? 0 : FILE_SHARE_READ,
                0,
                (WRITE == mode) ? CREATE_ALWAYS : OPEN_EXISTING,
                //FILE_ATTRIBUTE_NORMAL,
                //FILE_FLAG_NO_BUFFERING |
                FILE_FLAG_SEQUENTIAL_SCAN,
                0);
            if (INVALID_HANDLE_VALUE == _p->f)
            {
                throw Error(
                    "djv::Core::FileIO",
                    errorLabels()[ERROR_OPEN].
                    arg(QDir::toNativeSeparators(fileName)));
            }
#else // DJV_WINDOWS
            int readFlag = 0;
#if defined(DJV_LINUX)
            //readFlag = O_DIRECT;
#endif // DJV_LINUX
            _p->f = ::open(
                fileName.toUtf8().data(),
                (WRITE == mode) ?
                (O_WRONLY | O_CREAT | O_TRUNC) : (O_RDONLY | readFlag),
                (WRITE == mode) ?
                (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) : (0));
            if (-1 == _p->f)
            {
                throw Error(
                    "djv::Core::FileIO",
                    errorLabels()[ERROR_OPEN].
                    arg(QDir::toNativeSeparators(fileName)));
            }
#endif // DJV_WINDOWS

            _p->fileName = fileName;

            // File position and size.
            _p->pos = 0;
#if defined(DJV_WINDOWS)
            _p->size = ::GetFileSize(_p->f, 0);
#else // DJV_WINDOWS
            _p->size = FileInfo(fileName).size();
#endif // DJV_WINDOWS
            //DJV_DEBUG_PRINT("size = " << _p->size);

            // I/O mode.
            _p->mode = mode;

            // Memory mapping.
#if defined(DJV_MMAP)
            if (READ == _p->mode && _p->size > 0)
            {
                //DJV_DEBUG_PRINT("mmap");
#if defined(DJV_WINDOWS)
                _p->mmap = ::CreateFileMapping(_p->f, 0, PAGE_READONLY, 0, 0, 0);
                if (!_p->mmap)
                {
                    throw Error(
                        "djv::Core::FileIO",
                        errorLabels()[ERROR_FILE_MAPPING].
                        arg(QDir::toNativeSeparators(_p->fileName)).
                        arg(ErrorUtil::lastError()));
                }
                _p->mmapStart = reinterpret_cast<const quint8 *>(
                    ::MapViewOfFile(_p->mmap, FILE_MAP_READ, 0, 0, 0));
                if (!_p->mmapStart)
                {
                    throw Error(
                        "djv::Core::FileIO",
                        errorLabels()[ERROR_MEMORY_MAP].
                        arg(QDir::toNativeSeparators(_p->fileName)).
                        arg(ErrorUtil::lastError()));
                }
                _p->mmapEnd = _p->mmapStart + _p->size;
                _p->mmapP = _p->mmapStart;
#else // DJV_WINDOWS
        //DJV_DEBUG_PRINT("mmap 2");
                _p->mmap = ::mmap(0, _p->size, PROT_READ, MAP_SHARED, _p->f, 0);
                if (_p->mmap == (void *)-1)
                {
                    throw Error(
                        "djv::Core::FileIO",
                        errorLabels()[ERROR_MEMORY_MAP].
                        arg(QDir::toNativeSeparators(_p->fileName)));
                }
                _p->mmapStart = reinterpret_cast<const quint8 *>(_p->mmap);
                _p->mmapEnd = _p->mmapStart + _p->size;
                _p->mmapP = _p->mmapStart;
#endif // DJV_WINDOWS
            }
#endif // DJV_MMAP
        }

        void FileIO::close()
        {
            //DJV_DEBUG("FileIO::close");

#if defined(DJV_MMAP)
#if defined(DJV_WINDOWS)
            if (_p->mmapStart != 0)
            {
                if (!::UnmapViewOfFile((void *)_p->mmapStart))
                {
                    //DJV_DEBUG_PRINT("UnmapViewOfFile error");
                }
                _p->mmapStart = 0;
            }
            if (_p->mmap != 0)
            {
                if (!::CloseHandle(_p->mmap))
                {
                    //DJV_DEBUG_PRINT("CloseHandle error");		
                }
                _p->mmap = 0;
            }
#else // DJV_WINDOWS
            _p->mmapStart = 0;
            if (_p->mmap != (void *)-1)
            {
                //DJV_DEBUG_PRINT("munmap");
                int r = ::munmap(_p->mmap, _p->size);
                if (-1 == r)
                {
                    //const QString err(::strerror(errno));
                    //DJV_DEBUG_PRINT("errno = " << err);
                }
                _p->mmap = (void *)-1;
            }
#endif // DJV_WINDOWS
            _p->mmapEnd = 0;
            _p->mmapP = 0;
#endif // DJV_MMAP
            _p->fileName.clear();
#if defined(DJV_WINDOWS)
            if (_p->f != INVALID_HANDLE_VALUE)
            {
                ::CloseHandle(_p->f);
                _p->f = INVALID_HANDLE_VALUE;
            }
#else // DJV_WINDOWS
            if (_p->f != -1)
            {
                //DJV_DEBUG_PRINT("close");
                int r = ::close(_p->f);
                if (-1 == r)
                {
                    const QString err(::strerror(errno));
                    //DJV_DEBUG_PRINT("errno = " << err);
                }
                _p->f = -1;
            }
#endif // DJV_WINDOWS
            _p->pos = 0;
            _p->size = 0;
            _p->mode = static_cast<MODE>(0);
        }

        const QString & FileIO::fileName() const
        {
            return _p->fileName;
        }

        bool FileIO::isValid() const
        {
            return
#if defined(DJV_WINDOWS)
                _p->f != INVALID_HANDLE_VALUE &&
#else
                _p->f != -1 &&
#endif
                (_p->size ? _p->pos < _p->size : true);
        }

        quint64 FileIO::size() const
        {
            return _p->size;
        }

        void FileIO::get(void * in, quint64 size, int wordSize)
        {
            //DJV_DEBUG("FileIO::get");
            //DJV_DEBUG_PRINT("size = " << size);
            //DJV_DEBUG_PRINT("word size = " << wordSize);

#if defined(DJV_MMAP)
            const quint8 * p = _p->mmapP + size * wordSize;
            if (p > _p->mmapEnd)
            {
                throw Error(
                    "djv::Core::FileIO",
                    errorLabels()[ERROR_READ].
                    arg(QDir::toNativeSeparators(_p->fileName)));
            }
            if (_p->endian && wordSize > 1)
            {
                Memory::convertEndian(_p->mmapP, in, size, wordSize);
            }
            else
            {
                memcpy(in, _p->mmapP, size * wordSize);
            }
            _p->mmapP = p;
#else // DJV_MMAP
#if defined(DJV_WINDOWS)
            DWORD n;
            if (!::ReadFile(_p->f, in, size * wordSize, &n, 0))
            {
                throw Error(
                    "djv::Core::FileIO",
                    errorLabels()[ERROR_READ].
                    arg(QDir::toNativeSeparators(_p->fileName)));
            }
#else // DJV_WINDOWS
    //if (-1 == ::read(_p->f, in, size * wordSize) == (size * wordSize))
    //{
    //    throw Error(
    //        "djv::Core::FileIO",
    //        errorLabels()[ERROR_READ].
    //        arg(QDir::toNativeSeparators(_p->fileName)));
    //}
            ::read(_p->f, in, size * wordSize);
            if (_p->endian && wordSize > 1)
            {
                Memory::convertEndian(in, size, wordSize);
            }
#endif // DJV_WINDOWS
#endif // DJV_MMAP
            _p->pos += size * wordSize;
        }

        void FileIO::set(const void * in, quint64 size, int wordSize)
        {
            //DJV_DEBUG("FileIO::set");
            //DJV_DEBUG_PRINT("size = " << size);
            //DJV_DEBUG_PRINT("word size = " << wordSize);
            //DJV_DEBUG_PRINT("endian = " << _p->endian);

            quint8 * p = (quint8 *)in;
            std::vector<quint8> tmp;
            if (_p->endian && wordSize > 1)
            {
                tmp.resize(size * wordSize);
                p = tmp.data();
                Memory::convertEndian(in, p, size, wordSize);
            }

#if defined(DJV_WINDOWS)
            DWORD n;
            if (!::WriteFile(_p->f, p, static_cast<DWORD>(size * wordSize), &n, 0))
            {
                throw Error(
                    "djv::Core::FileIO",
                    errorLabels()[ERROR_WRITE].
                    arg(QDir::toNativeSeparators(_p->fileName)));
            }
#else
            if (::write(_p->f, p, size * wordSize) == -1)
            {
                throw Error(
                    "djv::Core::FileIO",
                    errorLabels()[ERROR_WRITE].
                    arg(QDir::toNativeSeparators(_p->fileName)));
            }
#endif

            _p->pos += size * wordSize;
            _p->size = Math::max(_p->pos, _p->size);
        }

        void FileIO::readAhead()
        {
#if defined(DJV_MMAP)
#if defined(DJV_LINUX)
            ::madvise((void *)_p->mmapStart, _p->size, MADV_WILLNEED);
#endif // DJV_LINUX
#else // DJV_MMAP
#if defined(DJV_LINUX)
            ::posix_fadvise(_p->f, 0, _p->size, POSIX_FADV_NOREUSE);
            ::posix_fadvise(_p->f, 0, _p->size, POSIX_FADV_WILLNEED);
#endif // DJV_LINUX
#endif // DJV_MMAP
        }

        const quint8 * FileIO::mmapP() const
        {
            return _p->mmapP;
        }

        const quint8 * FileIO::mmapEnd() const
        {
            return _p->mmapEnd;
        }

        quint64 FileIO::pos() const
        {
            return _p->pos;
            /*quint64 out = 0;
            switch (_p->mode)
            {
                case READ:
        #if defined(DJV_MMAP)
                    out = static_cast<quint64>(
                        _p->mmapP - reinterpret_cast<const quint8 *>(_p->mmapStart));
                    break;
        #else
                    // Fall through...
        #endif
                case WRITE:
        #if defined(DJV_WINDOWS)
                    out = ::SetFilePointer(_p->f, 0, 0, FILE_CURRENT);
        #else
                    out = ::lseek(_p->f, 0, SEEK_CUR);
        #endif
                    break;
            }
            return out;*/
        }

        void FileIO::setPos(quint64 in)
        {
            setPos(in, false);
        }

        void FileIO::seek(quint64 in)
        {
            setPos(in, true);
        }

        bool FileIO::endian() const
        {
            return _p->endian;
        }

        void FileIO::setEndian(bool in)
        {
            _p->endian = in;
        }

        void FileIO::setPos(quint64 in, bool seek)
        {
            //DJV_DEBUG("FileIO::setPos");
            //DJV_DEBUG_PRINT("in = " << static_cast<int>(in));
            //DJV_DEBUG_PRINT("seek = " << seek);

            switch (_p->mode)
            {
            case READ:
#if defined(DJV_MMAP)
                if (!seek)
                {
                    _p->mmapP = reinterpret_cast<const quint8 *>(_p->mmapStart) + in;
                }
                else
                {
                    _p->mmapP += in;
                }
                if (_p->mmapP > _p->mmapEnd)
                {
                    throw Error(
                        "djv::Core::FileIO",
                        errorLabels()[ERROR_SET_POS].
                        arg(QDir::toNativeSeparators(_p->fileName)));
                }
                break;
#else
                // Fall through...
#endif
            case WRITE:
            {
#if defined(DJV_WINDOWS)
                if (!::SetFilePointer(
                    _p->f,
                    static_cast<LONG>(in),
                    0,
                    !seek ? FILE_BEGIN : FILE_CURRENT))
                {
                    throw Error(
                        "djv::Core::FileIO",
                        errorLabels()[ERROR_SET_POS].
                        arg(QDir::toNativeSeparators(_p->fileName)));
                }
#else // DJV_WINDOWS
                if (::lseek(_p->f, in, !seek ? SEEK_SET : SEEK_CUR) == (off_t)-1)
                {
                    throw Error(
                        "djv::Core::FileIO",
                        errorLabels()[ERROR_SET_POS].
                        arg(QDir::toNativeSeparators(_p->fileName)));
                }
#endif // DJV_WINDOWS
            }
            break;
            }
            if (!seek)
            {
                _p->pos = in;
            }
            else
            {
                _p->pos += in;
            }
        }

        const QStringList & FileIO::errorLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Core::FileIO", "Cannot open file: \"%1\"") <<
#       if defined(DJV_WINDOWS)
                qApp->translate("djv::Core::FileIO", "Cannot create file mapping: \"%1\" (%2)") <<
                qApp->translate("djv::Core::FileIO", "Cannot memory-map file: \"%1\" (%2)") <<
#       else // DJV_WINDOWS
                qApp->translate("djv::Core::FileIO", "Cannot memory-map file: \"%1\"") <<
#       endif // DJV_WINDOWS
                qApp->translate("djv::Core::FileIO", "Error reading file: \"%1\"") <<
                qApp->translate("djv::Core::FileIO", "Error writing file: \"%1\"") <<
                qApp->translate("djv::Core::FileIO", "Cannot set file position: \"%1\"");
            DJV_ASSERT(ERROR_COUNT == data.count());
            return data;
        }

    } // namespace Core
} // namespace djv
