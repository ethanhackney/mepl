#ifndef HASH_H
#define HASH_H

#include "lib.h"

struct hash;

struct hash *hash_new(void);
void        hash_free(struct hash **hpp, void (*dtor)(void *ptr));
void        hash_set(struct hash *hp, void *ptr);
void        hash_stat(const struct hash *hp, FILE *fp);
bool        hash_has(const struct hash *hp, const void *ptr);

#endif
