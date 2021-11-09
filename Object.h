#ifndef __OBJECT__H__SL
#define __OBJECT__H__SL
/**
**********************************************************************************************************************************************************************************************************************************
* @file:	object.h
* @author:	stan le
* @date:	03 Nov 2021 13:20:07 Wednesday
* @brief:	Object is a trivial class that can only be created, destroyed, compared, and displayed.
            It needs a class descriptor so we create a class descriptor metaclass named "Class".
            The two depends on each other, since the Class metaclass is a subset of the Object class
            too. We will initiate these two together specifically since they are the root of all
            the classes in our Object Oriented ANSI-C.
**********************************************************************************************************************************************************************************************************************************
**/

#include <stdio.h>
#include <stdarg.h>

extern const void* Object;  /* new(Object); */
extern const void* Class;   /* new(Class); */

/* Memory Management */
void* new (const void* class, ...);
void delete (void* self);

/* Other Class Methods */
int differ(const void* self, const void* b);
int puto(const void* self, FILE* file_pointer);

/* Statically linked methods */
const void* classOf(const void* self);
size_t sizeOf(const void* self);
const void* super(const void* self); /* class' superclass */


#endif //!__OBJECT__H__SL