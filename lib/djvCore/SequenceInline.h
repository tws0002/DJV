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

namespace djv
{
    namespace Core
    {
        inline qint64 Sequence::start() const
        {
            return frames.count() ? frames[0] : 0;
        }

        inline qint64 Sequence::end() const
        {
            return frames.count() ? frames[frames.count() - 1] : 0;
        }
        
        inline qint64 Sequence::stringToFrame(const QString & string, int * pad)
        {
            const int length = string.length();
            if (length && '#' == string[0])
                return -1;
            if (pad)
            {
                *pad = 0;
                int i = 0;
                if (length && '-' == string[i])
                {
                    ++i;
                }
                if ((length - i) > 1)
                {
                    if ('0' == string[i])
                    {
                        *pad = (length - i);
                    }
                }
            }
            return string.toLongLong();
        }

    } // namespace Core

    inline bool operator == (const Core::Sequence & a, const Core::Sequence & b)
    {
        return a.frames == b.frames && a.pad == b.pad && a.speed == b.speed;
    }

    inline bool operator != (const Core::Sequence & a, const Core::Sequence & b)
    {
        return !(a == b);
    }

} // namespace djv
