#ifndef __OBJECT_STRUCT__H__SL
#define __OBJECT_STRUCT__H__SL
/**
**********************************************************************************************************************************************************************************************************************************
* @file:	Object_struct.h
* @author:	stan le
* @date:	03 Nov 2021 16:15:10 Wednesday
* @brief:	A representation of the Object class. It is nothing but a pointer to its class descriptor.
**********************************************************************************************************************************************************************************************************************************
**/

#include <stdarg.h>
#include <stdio.h>


/**
 * @brief Object is such a minimal class that it only points to a class descriptor and that's it.
 * 
 */
struct Object
{
    const struct Class * class;
};

/**
 * @brief This is the representation of the METACLASS Class. A metaclass
 * is a class; therefore, its elements points to a class description.
 * Pointing to a class description is exactly what an Object can do, so
 * Class is a subclass of Object! 
 */
struct Class
{
    const struct Object _;
    const char * name;
    const struct Class * super;
    size_t size;
    void * (* ctor) (void * self, va_list * arg_list);
    void * (* dtor) (void * self);
    int (* differ) (const void * self, const void * b);
    int (* puto) (const void * self, FILE * file_pointer);
};

/**
 * These super selectors should only be called by a subclass implementation
 * therefore they are only declared in the representaion header.
 * we supply selectors for all dynamically linked methods to be safe.
**/

void * super_ctor (const void * class, void * self, va_list * arg_list);
void * super_dtor (const void * class, void * self);
int super_differ (const void * class, const void * self, const void * b);
int super_puto (const void * class, const void * self, FILE * file_pointer);

#endif  //!__OBJECT_IMP__H__SL