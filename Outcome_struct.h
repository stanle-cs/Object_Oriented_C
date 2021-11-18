#ifndef __OUTCOME_STRUCT__H__SL
#define __OUTCOME_STRUCT__H__SL

#include "../Object_struct.h"

struct Outcome
{
    const struct Object _;
    char * name;
    int odds;
};

#define name(p) (((const struct Outcome *)(p))->name)
#define odds(p) (((const struct Outcome *)(p))->odds)


#endif  /* !__OUTCOME_STRUCT__H__SL */