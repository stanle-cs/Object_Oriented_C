#include "Object_struct.h"

struct Point
{
    int x, y;
};

#define  x(p)  (((const struct Point *) (p))->x)
#define  y(p)  (((const struct Point *) (p))->y)

void super_draw(const void * class, const void * self);

struct PointClass
{
    const struct Class _;
    void (*draw) (const void * self);
};
