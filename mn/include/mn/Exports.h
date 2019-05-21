
#ifndef MN_EXPORT_H
#define MN_EXPORT_H

#ifdef MN_STATIC_DEFINE
#  define MN_EXPORT
#  define MN_NO_EXPORT
#else
#  ifndef MN_EXPORT
#    ifdef mn_EXPORTS
        /* We are building this library */
#      define MN_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define MN_EXPORT __declspec(dllimport)
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
#  define MN_DEPRECATED_EXPORT MN_EXPORT MN_DEPRECATED
#endif

#ifndef MN_DEPRECATED_NO_EXPORT
#  define MN_DEPRECATED_NO_EXPORT MN_NO_EXPORT MN_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef MN_NO_DEPRECATED
#    define MN_NO_DEPRECATED
#  endif
#endif

#endif /* MN_EXPORT_H */
