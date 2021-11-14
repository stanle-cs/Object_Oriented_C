#ifndef __OBJECT__H__SL
#define __OBJECT__H__SL

#include <stdio.h>
#include <stdarg.h>


extern const void * Object;
extern const void * Class;

void * new(const void * class, ...);
void delete(void * self);

void * ctor(void * self, va_list * arglist_ptr);
void * dtor(void * self);
int differ(const void * self, const void * other);
int puto(const void * self, FILE * file_pointer);

const void * class_of(const void * self);
const void * super(const void * self);
size_t size_of(const void * self);

#endif  /* !__OBJECT__H__SL */