/*
This file is a part of: LinaGX
https://github.com/inanevin/LinaGX

Author: Inan Evin
http://www.inanevin.com

The 2-Clause BSD License

Copyright (c) [2023-] Inan Evin

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef LINAGX_API_H
#define LINAGX_API_H

namespace LinaGX
{

#ifdef LINAGX_STATIC_DEFINE
#define LINAGX_API
#define LINAGX_NO_EXPORT
#else
#ifndef LINAGX_API
#ifdef LinaGX_EXPORTS
    /* We are building this library */
#define LINAGX_API
#else
    /* We are using this library */
#define LINAGX_API
#endif
#endif

#ifndef LINAGX_NO_EXPORT
#define LINAGX_NO_EXPORT
#endif
#endif

#ifndef LINAGX_DEPRECATED
#define LINAGX_DEPRECATED __declspec(deprecated)
#endif

#ifndef LINAGX_DEPRECATED_EXPORT
#define LINAGX_DEPRECATED_EXPORT LINAGX_API LINAGX_DEPRECATED
#endif

#ifndef LINAGX_DEPRECATED_NO_EXPORT
#define LINAGX_DEPRECATED_NO_EXPORT LINAGX_NO_EXPORT LINAGX_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#ifndef LINAGX_NO_DEPRECATED
#define LINAGX_NO_DEPRECATED
#endif
#endif

} // namespace LinaGX

#endif /* LINAGX_API_H */
