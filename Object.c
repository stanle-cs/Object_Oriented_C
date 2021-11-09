/**
**********************************************************************************************************************************************************************************************************************************
* @file:	Object.c
* @author:	Stan Le
* @date:	08 Nov 2021 17:29:13 Monday
* @brief:	The implementation for the base of the Object Oriented ANSI-C
**********************************************************************************************************************************************************************************************************************************
**/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>

#include "Object_struct.h"
#include "Object.h"

/**
 * Selectors
 * one argument _self is the object for dynamic linkage. We verify that
 * it exists and that the required method exists for that object.
 * We then call the method  and pass all arguments to it.
 * The result of the method called will be the result of the selector.
**/

void* ctor(void* _self, va_list* args_list_ptr)
{
    // get the class descriptor from the object _self passed in
    const struct Class* class = classOf(_self);
    assert(class->ctor);

    return class->ctor(_self, args_list_ptr);
}

void* dtor(void* _self)
{
    const struct Class* class = classOf(_self);
    assert(class->dtor);

    return class->dtor(_self);
}

int puto(const void* _self, FILE* fp)
{
    const struct Class* class = classOf(_self);

    assert(class->puto);
    return class->puto(_self, fp);
}

int differ(const void* _self, const void* b)
{
    const struct Class* class = classOf(_self);

    assert(class->differ);
    return class->differ(_self, b);
}

/**
 * Memory Management Methods
 * I put this under Selectors because these needs to call ctor and dtor
 * so I don't want to have to declare them before defining them
**/

void* new (const void* _class, ...)
{
    // cast _class to its real type: pointer to a class
    const struct Class* class = _class;
    // define a pointer to the new object we're creating
    struct Object* object;

    assert(class&& class->size);
    // allocate one struct Class space (this is why Class_ctor is
    // confident that it has enough space for name, size etc. later)
    object = calloc(1, class->size);

    assert(object);
    // object has only one field which is a pointer to a class
    // that will now point to that class descriptor we passed in
    // as _class (which was casted into the variable "class" earlier)
    object->class = class;

    // define the list of variables to pass to the ctor later
    va_list arg_list;
    va_start(arg_list, _class);
    object = ctor(object, &arg_list);
    va_end(arg_list);

    return object;
}

void delete (void* _self)
{
    if (_self)
        free(dtor(_self));
}

/**
 * Superclass Selectors
 * before a subclass constructor perform its own initialization, it has
 * to call the superclass constructor first.
 * after a subclass destructor reclaims all its resources it need to
 * call the superclass destructor to continue the reclaimation.
**/

void* super_ctor(const void* _class, void* _self, va_list* args_list_ptr)
{
    const struct Class* superclass = super(_class);
    assert(_self && superclass->ctor);

    return superclass->ctor(_self, args_list_ptr);
}

void* super_dtor(const void* _class, void* _self)
{
    const struct Class* superclass = super(_class);
    assert(_self && superclass->dtor);

    return superclass->dtor(_self);
}

int super_differ(const void* _class, const void* _self, const void* b)
{
    const struct Class* superclass = super(_class);

    assert(_self && superclass->differ);
    return superclass->differ(_self, b);
}

int super_puto(const void* _class, const void* _self, FILE* fp)
{
    const struct Class* superclass = super(_class);

    assert(_self && superclass->puto);
    return superclass->puto(_self, fp);
}

/**
 * These helper functions are available to outside files.
**/

const void* classOf(const void* _self)
{
    const struct Object* self = _self;
    assert(self && self->class);

    return self->class;
}

size_t sizeOf(const void* _self)
{
    const struct Class* class = classOf(_self);

    return class->size;
}

const void* super(const void* _self)
{
    const struct Class* self = _self;
    assert(self && self->super);

    return (self->super);
}

/**
 * Object class methods implementation
 * Object us the simplest class which can be created, destroyed,
 * displayed or check if differ.
 * Functions marked as static are ironically dynamically linked through
 * the use of selectors, thus they are marked to make sure they can't
 * be accessed outside this implementation file.
 * The selectors are available so they can be accessed by other files.
**/

static void* Object_ctor(void* _self, va_list* arg_list)
{
    return _self;
}

static void* Object_dtor(void* _self)
{
    return _self;
}

static int Object_differ(const void* _self, const void* b)
{
    return (_self != b);
}

static int Object_puto(const void* _self, FILE* file_pointer)
{
    const struct Class* class = classOf(_self);

    return fprintf(file_pointer, "%s at %p\n", class->name, _self);
}

/**
 * "Class" metaclass methods implenentation
 * Class is a metaclass that can only be created, the dtor of class
 * will return a NULL value to make sure it cannot be deleted.
 * Class is even simpler than Object eventhough it is a subclass of
 * Object.
 * Functions marked as static are ironically dynamically linked through
 * the use of selectors, thus they are marked to make sure they can't
 * be accessed outside this implementation file.
**/

static void* Class_ctor(void* _self, va_list* arg_pointer)
{
    struct Class* self = _self;

    self->name = va_arg(*arg_pointer, char*);
    self->super = va_arg(*arg_pointer, struct Class*);
    self->size = va_arg(*arg_pointer, size_t);
    assert(self->super);

    // Inheritance - copy the constructor and all other methods from
    // the super class description at super to new class description
    // at self:
    const size_t offset = offsetof(struct Class, ctor);
    memcpy((char*)self + offset, (char*)self->super + offset, sizeOf(self->super) - offset);

    // since function pointer and generic pointer may have different size,
    // we can't cast between them directly.
    // we need to create an intermediate "type" to hold a function pointer
    // this magical definition is for a type of function pointer to
    // functions that do not return anything
    typedef void (*ptr_to_void_func_type)();
    ptr_to_void_func_type selector;
    // create a new va_list which is a copy of the va_list that arg_pointer point to
    // so when we use arg_list instead of *arg_pointer, the original list won't be
    // increasing. We do this so that this list of args can be reused.
    va_list arg_list = *arg_pointer;

    // go through the list of given methods in arg_pointer
    // if we find a function listed, overwrite the original one
    // we just copied from super with that new function pointer
    while ((selector = va_arg(arg_list, ptr_to_void_func_type)))
    {
        ptr_to_void_func_type method = va_arg(arg_list, ptr_to_void_func_type);
        if (selector == (ptr_to_void_func_type)ctor)
        {
            *(ptr_to_void_func_type*)&self->ctor = method;
        }
        else if (selector == (ptr_to_void_func_type)dtor)
        {
            *(ptr_to_void_func_type*)&self->dtor = method;
        }
        else if (selector == (ptr_to_void_func_type)differ)
        {
            *(ptr_to_void_func_type*)&self->differ = method;
        }
        else if (selector == (ptr_to_void_func_type)puto)
        {
            *(ptr_to_void_func_type*)&self->puto = method;
        }
    }

    return self;
}

static void* Class_dtor(void* _self)
{
    struct Class* self = _self;

    fprintf(stderr, "%s: cannot destroy class\n", self->name);
    return 0;
}


/**
 * Initialization
 * As Class requires Object as a superclass while Object requires Class
 * too for its initiaion, but at this point they are both not initialized,
 * we will manually initialize them both in a struct Class
**/

static const struct Class object[] = {
    /**
     * Since there is no struct Class already initialized for struct
     * Object to point to, we will just initialize one for it instead
     * of passing it a pointer
     * this is a struct Class named object[0] or object for short
    **/
    {
        /* These params correspond to: */
        {object + 1},          /* const struct Object _ */
        "Object",              /* const char * name  */
        object,                /* const struct Class * super */
        sizeof(struct Object), /* size_t size */
        Object_ctor,           /* void* (*ctor) */
        Object_dtor,           /* void* (*dtor) */
        Object_differ,         /* int (*differ) */
        Object_puto,           /* int (*puto) */
    },
    {
        {object + 1},
        "Class",
        object,
        sizeof(struct Class),
        Class_ctor,
        Class_dtor,
        Object_differ,
        Object_puto,
     }, 
};
// now we can use object[0] and object[1] as "Object" and "Class".
const void* Object = object;
const void* Class = object + 1;
