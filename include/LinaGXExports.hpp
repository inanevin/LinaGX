
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
