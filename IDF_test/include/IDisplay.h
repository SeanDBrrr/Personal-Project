#ifndef IDISPLAY_H
#define IDISPLAY_H
#include "stdio.h"
#include "cstring"

#ifdef __cplusplus
extern "C" {
#endif

class IDisplay
{
public:
    virtual ~IDisplay() = default;
    virtual void init(int, int, uint8_t) = 0;
    virtual void display(void*) = 0;
};

#ifdef __cplusplus
}
#endif
#endif