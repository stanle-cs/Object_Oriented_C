#ifndef __POINT_STRUCT__H__SL
#define __POINT_STRUCT__H__SL

#include "Object_struct.h"

/******************************************************************************
 * Point structure
*******************************************************************************/
/* Needs to have a member that is a struct of its superclass */
struct Point
{
    const struct Object _;
    int x, y;
};

#define  x(p)  (((const struct Point *) (p))->x)
#define  y(p)  (((const struct Point *) (p))->y)

/******************************************************************************
 * PointClass metaclass
*******************************************************************************/
/* Used to add more meaningful methods to the class other than the ones offered
by the base class metaclass. */
struct PointClass
{
    const struct Class _;
    void (*draw) (const void * self);
};

/* Make the super_draw method visible so that subclass of this class can call it */
void super_draw(const void * class, const void * self);

#endif  /* !__POINT_STRUCT__H__SL */

