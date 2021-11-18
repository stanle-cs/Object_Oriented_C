#ifndef __CIRCLE_STRUCT__H__SL
#define __CIRCLE_STRUCT__H__SL

#include "Point_struct.h"

struct Circle
{
    const struct Point _;
    int radius;
};
#define radius(p)   (((const struct Circle *)(p))->radius)

#endif  /* !__CIRCLE_STRUCT__H__SL */