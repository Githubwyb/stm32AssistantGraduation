#ifndef _KEY_H_
#define _KEY_H_

#include "types.h"

typedef enum {
    KEY_UP,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_NUM
} KEY_ENUM;

typedef enum {
    KEY_PRESS,
    KEY_RELEASE
} KEY_STATE_ENUM;

typedef struct {
    KEY_ENUM key;
    KEY_STATE_ENUM state;
} KEY_MSG;

int key_detectProc(void);

#endif //! _KEY_H_
