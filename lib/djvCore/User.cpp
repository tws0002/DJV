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

#include <djvCore/User.h>

#include <djvCore/StringUtil.h>

#if defined(DJV_WINDOWS)
#include <windows.h>
#else
#include <pwd.h>
#include <unistd.h>
#endif

namespace djv
{
    namespace Core
    {
        User::~User()
        {}

        QString User::current()
        {
            QString out;
#if defined(DJV_WINDOWS)
            WCHAR tmp[StringUtil::cStringLength] = { 0 };
            DWORD size = StringUtil::cStringLength;
            ::GetUserNameW(tmp, &size);
            out = QString::fromWCharArray(tmp, size);
#else // DJV_WINDOWS
            out = uidToString(::getuid());
#endif // DJV_WINDOWS
            return out;
        }

        QString User::uidToString(uid_t value)
        {
            QString out;
#if ! defined(DJV_WINDOWS)
            struct passwd * buf = ::getpwuid(value);
            if (buf)
            {
                out = buf->pw_name;
            }
#endif // ! DJV_WINDOWS
            return out;
        }

    } // namespace Core
} // namespace djv
