#include <assert.h>

#include "Point.h"
#include "Point_struct.h"

/******************************************************************************
 * STATIC METHODS
*******************************************************************************/

void move(void * _point, int dx, int dy)
{
    struct Point * point = _point;
    point->x = x(point) + dx;
    point->y = y(point) + dy;
}

/******************************************************************************
 * GENERIC SELECTORS
******************************************************************************/

void draw(const void * _self)
{
    const struct PointClass * class = class_of(_self);

    assert(class->draw);
    class->draw(_self);
}

/******************************************************************************
 * SUPERCLASS SELECTORS
*******************************************************************************/

void super_draw(const void * _class, const void * _self)
{
    const struct PointClass * superclass = super(_class);

    assert(_self && superclass->draw);

    superclass->draw(_self);
}

/******************************************************************************
 * POINT CLASS METHODS
*******************************************************************************/

static void * Point_ctor(void * _self, va_list * arglist_ptr)
{
    struct Point * self = super_ctor(Point, _self, arglist_ptr);

    self->x = va_arg(*arglist_ptr, int);
    self->y = va_arg(*arglist_ptr, int);

    return self;
}

static void Point_draw(const void * _self)
{
    const struct Point * self = _self;

    printf("\".\" at %d,%d\n", self->x, self->y);
}

/******************************************************************************
 * POINTCLASS METACLASS
*******************************************************************************/
/* Call class constructor to create a list of dynamically linked methods of ctor,
 * dtor, differ and puto. Then add the draw function pointer to the draw spot
 * on the PointClass descriptor we're working on. (refer to Point_struct.h for
 * the full PointClass struct. However it is just an extension of the Class struct
 * since it only has one Class struct member and then a pointer for the draw method) */

static void * PointClass_ctor(void * _self, va_list * arglist_ptr)
{
    struct PointClass * self = super_ctor(PointClass, _self, arglist_ptr);

    typedef void (*funcptr)();
    funcptr selector;

    va_list cpy_arglist;
    va_copy(cpy_arglist, *arglist_ptr);

    while ((selector = va_arg(cpy_arglist, funcptr)))
    {
        funcptr method = va_arg(cpy_arglist, funcptr);
        if (selector == (funcptr) draw)
            *(funcptr*) &self->draw = method;
    }
    va_end(cpy_arglist);
    return self;
}

/******************************************************************************
 * INITIALIZATION
*******************************************************************************/

const void * PointClass;
const void * Point;

void initPoint(void)
{
    if (!PointClass)
    {
        PointClass = new(
            Class,
            "PointClass",
            Class,
            sizeof(struct PointClass),
            ctor, PointClass_ctor, NULL);
    }
    if (!Point)
    {
        Point = new(
            PointClass,
            "Point",
            Object,
            sizeof(struct Point),
            ctor, Point_ctor,
            draw, Point_draw, NULL);
    }
}