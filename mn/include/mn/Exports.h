
#ifndef API_MN_H
#define API_MN_H

#ifdef MN_STATIC_DEFINE
#  define API_MN
#  define MN_NO_EXPORT
#else
#  ifndef API_MN
#    ifdef mn_EXPORTS
        /* We are building this library */
#      define API_MN __declspec(dllexport)
#    else
        /* We are using this library */
#      define API_MN __declspec(dllimport)
#    endif
#  endif

#  ifndef MN_NO_EXPORT
#    define MN_NO_EXPORT 
#  endif
#endif

#ifndef MN_DEPRECATED
#  define MN_DEPRECATED __declspec(deprecated)
#endif

#ifndef MN_DEPRECATED_EXPORT
#  define MN_DEPRECATED_EXPORT API_MN MN_DEPRECATED
#endif

#ifndef MN_DEPRECATED_NO_EXPORT
#  define MN_DEPRECATED_NO_EXPORT MN_NO_EXPORT MN_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef MN_NO_DEPRECATED
#    define MN_NO_DEPRECATED
#  endif
#endif

#endif /* API_MN_H */
