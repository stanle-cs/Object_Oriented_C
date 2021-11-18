#include <assert.h>
#include "Circle.h"
#include "Circle_struct.h"

/* Circle class methods */

static void * Circle_ctor(void * _self, va_list * arglist_ptr)
{
    struct Circle * self = super_ctor(Circle, _self, arglist_ptr);

    self->radius = va_arg(*arglist_ptr, int);

    return self;

}

static void Circle_draw(const void * _self)
{
    const struct Circle * self = _self;

    printf("Circle at %d,%d radius %d\n", x(self), y(self), radius(self));
}

const void * Circle;

void initCircle(void)
{
    initPoint();
    if (!Circle)
    {
        Circle = new(
            PointClass,
            "Circle",
            Point,
            sizeof(struct Circle),
            ctor, Circle_ctor,
            draw, Circle_draw,
            NULL
            );
    }
}