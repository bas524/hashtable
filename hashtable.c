// ----------------------------------------------------------------------------
#include "hashtable.h"
#include "fasthash.h"
#include <string.h>
#include <stdio.h>

// ----------------------------------------------------------------------------
static char *mystrdup(const char *s) {
  char *b;
  if (!s) {
    return NULL;
  }
  if (!(b = (char *)malloc(strlen(s) + 1))) return NULL;
  strcpy(b, s);
  return b;
}
// ----------------------------------------------------------------------------
static hash_size def_hashfunc(const char *key) {
  hash_size hash = 0;
  if (!key) {
    return -1;
  }

  hash = (hash_size)SuperFastHash(key, strlen(key));

  return hash;
}
// ----------------------------------------------------------------------------
LIBHASH_API HASHTBL *hashtbl_create(hash_size size, hash_size(*hashfunc)(const char *)) {
  HASHTBL *hashtbl;

  if (size < 1) {
    return NULL;
  }

  if (!(hashtbl = (HASHTBL *)malloc(sizeof(HASHTBL)))) {
    return NULL;
  }
  memset(hashtbl, 0, sizeof(HASHTBL));

  if (!(hashtbl->nodes = (struct hashnode_s **)calloc(size, sizeof(struct hashnode_s *)))) {
    free(hashtbl);
    return NULL;
  }

  hashtbl->size = size;

  if (hashfunc) {
    hashtbl->hashfunc = hashfunc;
  }
  else {
    hashtbl->hashfunc = def_hashfunc;
  }

  return hashtbl;
}
// ----------------------------------------------------------------------------
LIBHASH_API void hashtbl_destroy(HASHTBL *hashtbl) {
  hash_size n;
  struct hashnode_s *node, *oldnode;
  if (!hashtbl) {
    return;
  }

  for (n = 0; n < hashtbl->size; ++n) {
    node = hashtbl->nodes[n];
    while (node) {
      free(node->key);
      oldnode = node;
      node = node->next;
      free(oldnode);
    }
  }
  free(hashtbl->nodes);
  free(hashtbl);
}
// ----------------------------------------------------------------------------
LIBHASH_API int hashtbl_insert(HASHTBL *hashtbl, const char *key, void *data) {
  struct hashnode_s *node = NULL;
  hash_size          hash;
  int                nodecnt = 0;

  if (!hashtbl) {
    return -1;
  }

  hash = hashtbl->hashfunc(key) % hashtbl->size;

  node = hashtbl->nodes[hash];
  while (node) {
    if (!strcmp(node->key, key)) {
      node->data = data;
      return 0;
    }
    node = node->next;
    if (nodecnt == 5) {
      hashtbl_resize(hashtbl, hashtbl->size * 2);
    }
    nodecnt++;
  }

  if (!(node = (struct hashnode_s *)malloc(sizeof(struct hashnode_s)))) {
    return -1;
  }
  memset(node, 0, sizeof(struct hashnode_s));

  if (!(node->key = mystrdup(key))) {
    free(node);
    return -1;
  }
  node->data = data;
  node->next = hashtbl->nodes[hash];
  hashtbl->nodes[hash] = node;
  hashtbl->valid_count++;

  return 0;
}
// ----------------------------------------------------------------------------
LIBHASH_API int hashtbl_remove(HASHTBL *hashtbl, const char *key) {
  struct hashnode_s *node, *prevnode = NULL;
  hash_size          hash;
  if (!hashtbl) {
    return -1;
  }

  hash = hashtbl->hashfunc(key) % hashtbl->size;

  node = hashtbl->nodes[hash];
  while (node) {
    if (!strcmp(node->key, key)) {
      free(node->key);
      if (prevnode) {
        prevnode->next = node->next;
      }
      else {
        hashtbl->nodes[hash]   = node->next;
      }
      free(node);
      hashtbl->valid_count--;
      return 0;
    }
    prevnode = node;
    node = node->next;
  }

  return -1;
}
// ----------------------------------------------------------------------------
LIBHASH_API void *hashtbl_get(HASHTBL *hashtbl, const char *key) {
  struct hashnode_s *node;
  hash_size          hash;
  if (!hashtbl) {
    return NULL;
  }

  hash = hashtbl->hashfunc(key) % hashtbl->size;

  node = hashtbl->nodes[hash];
  while (node) {
    if (!strcmp(node->key, key)) return node->data;
    node = node->next;
  }

  return NULL;
}
// ----------------------------------------------------------------------------
LIBHASH_API int hashtbl_resize(HASHTBL *hashtbl, hash_size size) {
  HASHTBL newtbl;
  hash_size n;
  struct hashnode_s *node, *next;

  if (!hashtbl) {
    hashtbl = (HASHTBL *)malloc(sizeof(HASHTBL));
    if (!hashtbl) {
      return -1;
    }
    memset(hashtbl, 0, sizeof(HASHTBL));
    hashtbl->hashfunc = def_hashfunc;
  }

  newtbl.size            = size;
  newtbl.valid_count     = hashtbl->valid_count;
  newtbl.hashfunc        = hashtbl->hashfunc;

  if (!(newtbl.nodes = (struct hashnode_s **)calloc(size, sizeof(struct hashnode_s *)))) return -1;

  for (n = 0; n < hashtbl->size; ++n) {
    for (node = hashtbl->nodes[n]; node; node = next) {
      next = node->next;
      hashtbl_insert(&newtbl, node->key, node->data);
      hashtbl_remove(hashtbl, node->key);

    }
  }

  free(hashtbl->nodes);
  hashtbl->size        = newtbl.size;
  hashtbl->nodes       = newtbl.nodes;
  hashtbl->valid_count = newtbl.valid_count;

  return 0;
}
// ----------------------------------------------------------------------------
LIBHASH_API hash_size hashtbl_get_index(HASHTBL *hashtbl, const char *key) {
  struct hashnode_s *node;
  hash_size          hash;

  if (!hashtbl) {
    return (hash_size) - 1;
  }
  /*  fprintf(stderr, "hashtbl_get() key=%s, hash=%d\n", key, hash);*/
  hash = hashtbl->hashfunc(key) % hashtbl->size;
  node = hashtbl->nodes[hash];
  while (node) {
    if (!strcmp(node->key, key)) return hash;
    node = node->next;
  }

  return (hash_size) - 1;
}
// ----------------------------------------------------------------------------
LIBHASH_API hash_size hashtbl_get_valid_count(HASHTBL *hashtbl) {
  if (hashtbl) {
    return hashtbl->valid_count;
  }
  return -1;
}
// ----------------------------------------------------------------------------
LIBHASH_API hash_size /*UPHP_CALL*/ hashtbl_get_hash(char *key, int key_len) {
  return SuperFastHash(key, key_len);
}
