#include <assert.h>     /* for assert() */
#include <stddef.h>     /* for offsetof() */
#include <string.h>     /* for memcpy() */
#include <stdlib.h>     /* for calloc() */

#include "Object.h"
#include "Object_struct.h"

/**
 * STATIC METHODS
 * These helper functions are available to outside files too.
 * Since these are declared in Object.h, any
***/

const void * class_of(const void * _self)
{
    const struct Object * self = _self;
    assert(self && self->class);

    return self->class;
}

size_t size_of(const void * _self)
{
    const struct Class * class = class_of(_self);

    return class->size;
}

const void * super(const void * _self)
{
    const struct Class * self = _self;
    assert(self && self->super);

    return self->super;
}

/**
 * GENERIC SELECTORS
 * Functions that are used to call the other methods defined in the class descriptor
***/

/**
 * @brief Selector function that will call the actual ctor function defined in the class.
 *        Ctor aids in the creation of a new object
 *
 * @param _self the object to call it for
 * @param arg_list_ptr the optional list of arguments
 * @return void* return a generic pointer pointing to the constructed object
 */
void * ctor(void * _self, va_list * arg_list_ptr)
{
    const struct Class * class = class_of(_self);
    assert(class->ctor);

    return class->ctor(_self, arg_list_ptr);
}

/**
 * @brief Selector function that will call the actual destructor given by the class descriptor.
 *        Dtor aids in the reclaimation of memory when destroying an object
 *
 * @param _self the object to call it for
 * @return void* return a generic pointer pointing to the object for further deconstruction
 */
void * dtor(void * _self)
{
    const struct Class * class = class_of(_self);
    assert(class->dtor);

    return class->dtor(_self);
}

/**
 * @brief Selector fuctions to call the actual differ function defined in the class descriptor.
 *        Differ check if two objects are different from each other.
 *
 * @param _self the object to call it for
 * @param other the object to compare
 * @return int return the value returned by the class-defined differ function
 */
int differ(const void * _self, const void * other)
{
    const struct Class * class = class_of(_self);
    assert(class->differ);

    return class->differ(_self, other);
}

/**
 * @brief Selector function to call the actual puto function defined by the class descriptor.
 *        Puto prints the object out to a file.
 *
 * @param _self the object to print
 * @param file_ptr the file to print into
 * @return int the value returned by the class-defined puto function
 */
int puto(const void * _self, FILE * file_ptr)
{
    const struct Class * class = class_of(_self);
    assert(class->puto);

    return class->puto(_self, file_ptr);
}

/**
 * MEMORY MANAGEMENT FUNCTIONS
 * new and delete are called whenever a new object is created or destroyed
***/

void * new(const void * _class, ...)
{
    const struct Class * class = _class;
    assert(class && class->size);

    struct Object * object = NULL;
    object = calloc(1, class->size);
    assert(object);
    object->class = class;

    va_list arg_list;
    va_start(arg_list, _class);
    object = ctor(object, &arg_list);
    va_end(arg_list);

    return object;
}

void delete(void * _self)
{
    if (_self)
        free(dtor(_self));
    _self = NULL;
}

/**
 * SUPER CLASS SELECTORS
 * Functions that are called by any subclasses to access its superclass methods.
***/

void * super_ctor(const void * _class, void * _self, va_list * arg_list_ptr)
{
    const struct Class * superclass = super(_class);
    assert(_self && superclass->ctor);

    return superclass->ctor(_self, arg_list_ptr);
}

void * super_dtor(const void * _class, void * _self)
{
    const struct Class * superclass = super(_class);
    assert(_self && superclass->dtor);

    return superclass->dtor(_self);
}

int super_differ(const void * _class, const void * _self, const void* other)
{
    const struct Class* superclass = super(_class);

    assert(_self && superclass->differ);
    return superclass->differ(_self, other);
}

int super_puto(const void * _class, const void * _self, FILE * file_ptr)
{
    const struct Class * superclass = super(_class);

    assert(_self && superclass->puto);
    return superclass->puto(_self, file_ptr);
}

/**
 * OBJECT CLASS METHODS
 * Methods that are unique to the Object class. Since Object is the base class of everything,
 * its method is the default of all its subclasses, including the Class metaclass.
***/

/**
 * @brief By the time the contructor of Object is called, the new() method has already created
 * a pointer to the object and allocated a chunk of memory to it. It has also copied the class
 * descriptor pointer into the only field that Object has, the "class" field. Therefore there
 * is nothing for Object_ctor to do.
 *
 * @param _self the pointer to the Object
 * @param arglist_ptr the pointer to the optional list of arguments, will always be empty for
 *                    Object since we don't have extra data aside from the class descriptor.
 * @return void* the pointer to the object
 */
static void * Object_ctor(void * _self, va_list * arglist_ptr)
{
    return _self;
}

/**
 * @brief "Delete" all the data in Object. Since Object has only one data field which is
 * the class descriptor pointer, which is the responsibility of delete(), we do nothing here
 *
 * @param _self pointer to the Object to be destructed
 */
static void * Object_dtor(void * _self)
{
    return _self;
}

static int Object_differ(const void * _self, const void * other)
{
    return (_self != other);
}

static int Object_puto(const void * _self, FILE * file_ptr)
{
    const struct Class * class = class_of(_self);

    return fprintf(file_ptr, "%s at %p\n", class->name, _self);
}

/**
 * CLASS METACLASS METHODS
 * Methods that are unique to the Class metaclass. It is only used to create a new class
 * descriptor so it only really ctor method, the dtor method is there to ensure that we
 * prevent accidental deletion of class descriptors.
***/

static void * Class_ctor(void * _self, va_list * arglist_ptr)
{
    /* "self" is a class descriptor so cast it as such */
    struct Class * self = _self;

    /* pass the values in the arg list into self */
    self->name = va_arg(*arglist_ptr, char *);
    self->super = va_arg(*arglist_ptr, struct Class *);
    self->size = va_arg(*arglist_ptr, size_t);
    assert(self->super);

    /* Now is the hard part, we need to pass all the methods into
    their correct position on the class descriptor */
    /* First find the offset of the ctor method inside the class struct */
    const size_t offset = offsetof(struct Class, ctor);
    /* Here we assume that the superclass descriptor has been constructed.
    if so we can copy all of its methods to this newly created class descriptor
    first, then later we can just modify the ones we define in the arg_list */
    /**
     * Here we have:
     * (char *) self + offset : address of self + offset is the destination to copy the
     *   data to. Casted to char pointer because char is guaranteed to be 1 byte, therefore
     *   its pointer arithmetic will be equivalent to pointer-to-byte arithmetic. As in,
     *   when you add or substract 1 to the pointer, the address is moved 1 byte forward or
     *   backward.
     * (char *) self->super + offset: the data come from the super class descriptor starting
     *   at offset.
     * size_of(self->super) - offset: size_of(self->super) is the size of the entire
     *   superclass descriptor. Since we only need to copy from the position of offset
     *   to the end, we substract offset from the total size to get the correct chunk.
    **/
    memcpy((char *) self + offset, (char *) self->super + offset, size_of(self->super) - offset);

    /* pointer to any type usually have the same size, except for the pointer to function. on
    some system, the pointer to function may have different value since it's not part of the
    data section. Therefore casting between void *, aka generic pointer and a function pointer
    is not allowed. We can create a new type using typedef, and this new type will be in the
    type of a pointer to generic function */
    typedef void (*func_ptr)();
    func_ptr selector;

    /* copy the arglist to a new one so we can reuse the arglist_ptr multiple times */
    va_list cpy_arglist;
    va_copy(cpy_arglist, *arglist_ptr);
    /* the data given in the arg_list is of this structure: general name, local implementation
    to  map each methods given to its correct slot we use an chain if statement here until we
    read NULL */
    while (selector = va_arg(cpy_arglist, func_ptr))
    {
        func_ptr method = va_arg(cpy_arglist, func_ptr);
        /* at this point selector holds the slot and method holds the local implementation */
        if (selector == (func_ptr) ctor)
            /* since self->ctor is of the type void* (*)(const void *, va_list *) we need to cast
            it to the (func_pointer) type, we can either do it this way or cast method to self->ctor
            type, but since we have a bunch of them with different footprints it's more convenience
            this way. */
            *((func_ptr *) &self->ctor) = method;
        else if (selector == (func_ptr) dtor)
            *((func_ptr*) &self->dtor) = method;
        else if (selector == (func_ptr) differ)
            *((func_ptr*) &self->differ) = method;
        else if (selector == (func_ptr) puto)
            *((func_ptr*) &self->puto) = method;
    }
    va_end(cpy_arglist);
    return self;
}

static void * Class_dtor(void * _self)
{
    struct Class * self = _self;
    fprintf(stderr, "%s: cannot destroy class", self->name);

    return NULL;
}

/**
 * INITIALIZATION
 * A very important step that requires special explaination. Each of the two members
 * of the struct is a class descriptor in itself, the first one for the Object class,
 * and the second one for the Class metaclass. The Object class needs a class descriptor,
 * therefore it needs a fully initialized Class metaclass. The Class metaclass in turn is
 * a subclass of the Object class and needs a fully initialized Object class. Therefore,
 * we need to be creative in how we solve this problem
***/

static const struct Class object[] = {
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

/* now we can use object[0] and object[1] as "Object" and "Class". */
const void* Object = object;
const void* Class = object + 1;
