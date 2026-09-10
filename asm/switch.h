#ifndef SWITCH_H
#define SWITCH_H

#ifdef __x86_64__
#include <arch/x86_64/switch.h>
#else
#include <arch/i386/switch.h>
#endif

#endif
