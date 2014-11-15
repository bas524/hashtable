#ifndef HASHTBL_H_INCLUDE_GUARD
#define HASHTBL_H_INCLUDE_GUARD

#include<stdlib.h>

#if defined (_WIN32) | defined (WIN32)
#define _WINNT
#else
#define _UNIX
#endif

#ifdef _WINNT
#define UPHP_CALL __cdecl
#ifndef LIBHASH_API
#if defined(UPHP_LIBHASH_LIBRARY)
#define LIBHASH_API extern "C" __declspec(dllexport)
#else
#define LIBHASH_API extern "C" __declspec(dllimport)
#endif
#endif
#endif /*WINNT*/

#ifdef _UNIX
#define UPHP_CALL
#ifndef LIBHASH_API
#if defined( __cplusplus)
#define LIBHASH_API extern "C"
#else
#if !defined(QT_GCC_SO)
#define LIBHASH_API extern
#else
#define LIBHASH_API extern "C"
#endif
#endif
#endif
#endif


#ifdef _WINNT
typedef unsigned __int32 hash_size;
#else
typedef uint32_t hash_size;
#endif

struct hashnode_s {
  char *key;
  void *data;
  struct hashnode_s *next;
};

typedef struct hashtbl {
  hash_size size;
  hash_size valid_count;
  struct hashnode_s **nodes;
  hash_size(*hashfunc)(const char *);
} HASHTBL;


LIBHASH_API HASHTBL  *hashtbl_create(hash_size size, hash_size(*hashfunc)(const char *));
LIBHASH_API void      hashtbl_destroy(HASHTBL *hashtbl);
LIBHASH_API int       hashtbl_insert(HASHTBL *hashtbl, const char *key, void *data);
LIBHASH_API int       hashtbl_remove(HASHTBL *hashtbl, const char *key);
LIBHASH_API void     *hashtbl_get(HASHTBL *hashtbl, const char *key);
LIBHASH_API int       hashtbl_resize(HASHTBL *hashtbl, hash_size size);
LIBHASH_API hash_size hashtbl_get_index(HASHTBL *hashtbl, const char *key);
LIBHASH_API hash_size hashtbl_get_valid_count(HASHTBL *hashtbl);

LIBHASH_API hash_size hashtbl_get_hash(char *key, int key_len);

#endif
