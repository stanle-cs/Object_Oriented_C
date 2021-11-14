#ifndef __OBJECT_STRUCT__H__SL
#define __OBJECT_STRUCT__H__SL

#include <stdio.h>
#include <stdarg.h>

struct Object
{
    const struct Class * class;
};

struct Class
{
    const struct Object _;
    const char * name;
    const struct Class * super;
    size_t size;
    void * (*ctor)(void * self, va_list * arg_ptr);
    void * (*dtor)(void * self);
    int (*differ)(const void * self, const void * other);
    int (*puto)(const void * self, FILE * file_ptr);
};

void * super_ctor(const void * class, void * self, va_list * arg_ptr);
void * super_dtor(const void * class, void * self);
int super_differ(const void * class, const void * self, const void * other);
int super_puto(const void * class, const void * self, FILE * file_ptr);

#endif  /* !__OBJECT_STRUCT__H__SL */
