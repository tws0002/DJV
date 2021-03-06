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

#include <djvCore/ErrorUtil.h>

#include <djvCore/Error.h>
#include <djvCore/StringUtil.h>
#include <djvCore/System.h>

#include <QCoreApplication>
#include <QString>

#if defined(DJV_WINDOWS)
#include <windows.h>
#endif // DJV_WINDOWS

namespace djv
{
    namespace Core
    {
        ErrorUtil::~ErrorUtil()
        {}

        QStringList ErrorUtil::format(const Error & error)
        {
            QStringList out;
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Core::ErrorUtil", "[ERROR] %1") <<
                qApp->translate("djv::Core::ErrorUtil", "[ERROR %1] %2");
            Q_FOREACH(const Error::Message & message, error.messages())
            {
                out += message.prefix.isEmpty() ?
                    data[0].arg(message.string) :
                    data[1].arg(message.prefix).arg(message.string);
            }
            return out;
        }

        QString ErrorUtil::lastError()
        {
            QString out;
#if defined(DJV_WINDOWS)
            struct Buffer
            {
                ~Buffer()
                {
                    LocalFree(p);
                }

                LPWSTR p = nullptr;

            } buffer;
            ::FormatMessageW(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                0,
                GetLastError(),
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                buffer.p,
                0,
                0);
            out = QString::fromWCharArray(buffer.p);
#endif // DJV_WINDOWS
            return out;
        }

        void ErrorUtil::print(const Error & error)
        {
            const QStringList list = format(error);
            for (int i = list.count() - 1; i >= 0; --i)
            {
                System::print(list[i]);
            }
        }

    } // namespace Core
} // namespace djv
