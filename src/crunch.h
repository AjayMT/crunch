
#ifndef _CRUNCH_H_
#define _CRUNCH_H_


#define DYLD_INTERPOSE(_replacement,_replacee)                          \
  __attribute__((used)) static struct{ const void* replacement; const void* replacee; } _interpose_##_replacee \
  __attribute__ ((section ("__DATA,__interpose"))) = { (const void*)(unsigned long)&_replacement, (const void*)(unsigned long)&_replacee };


void *crunch_malloc(size_t);
void crunch_free(void*);


#endif /* _CRUNCH_H_ */
