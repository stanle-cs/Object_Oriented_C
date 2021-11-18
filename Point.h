#ifndef __POINT__H__SL
#define __POINT__H__SL

#include "Object.h"

extern const void * Point;

/* New methods */

void draw(const void * self);
void move(void * point, int dx, int dy);

/* initPoint is used to set up the class descriptors for Point class */
void initPoint(void);

extern const void * PointClass;

#endif  /* !__POINT__H__SL */

