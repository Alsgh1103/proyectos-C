#ifndef __UTIL_H__
#define __UTIL_H__

#include "general/types.h"
#include "foreach.h"

template <typename T>
void intercambiar(T &a, T &b){
    T tmp = a;
    a = b;
    b = tmp;
}

TT fact(T n);

void DemoUtil();

#endif // __UTIL_H__