// SPDX-LICENSE-IDENTIFIER: MIT
#ifndef __ZDA_EXPORT_H__
#define __ZDA_EXPORT_H__

#if defined(__GNUC__) || defined(__clang__) || defined(__GUNG__)
#  define ZDA_EXPORT_ATTR __attribute__((visibility("default")))
#  define ZDA_IMPORT_ATTR __attribute__((visibility("default")))
#  define ZDA_NO_EXPORT   __attribute__((visibility("hidden")))
#elif defined(_MSC_VER)
#  define ZDA_EXPORT_ATTR __declspec(dllexport)
#  define ZDA_IMPORT_ATTR __declspec(dllimport)
#  define ZDA_NO_EXPORT
#endif //! defined(__GNUC__) ...

#ifdef ZDA_BUILD_SHARED
#  define ZDA_API    ZDA_EXPORT_ATTR
#  define ZDA_NO_API ZDA_NO_EXPORT
#elif ZDA_STATIC
#  define ZDA_API
#  define ZDA_NO_API
#else /* The shared user(don't specified anything is OK) */
#  define ZDA_API    ZDA_IMPORT_ATTR
#  define ZDA_NO_API ZDA_NO_EXPORT
#endif

#endif /* Header guard */