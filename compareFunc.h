#ifndef __COMPARE_H__
#define __COMPARE_H__
#include "general/types.h"

// C, C++
// typedef bool (*CompFunc)(const ContainerElemType &, const ContainerElemType &);
// C++11 en adelante

template <typename T>
bool Equal(const T &a, const T &b)
{ return a < b;  }

template <typename T>
bool Menor(const T &a, const T &b)
{ return a < b;  }

template <typename T>
bool Mayor(const T &a, const T &b)
{ return !(a==b) && !(a < b);  }




#endif // __COMPARE_H__