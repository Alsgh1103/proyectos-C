#ifndef __VARIADIC_UTIL__
#define __VARIADIC_UTIL__

template <typename Q>
auto Test(Q elem) { return elem;  }

template <typename Q, typename ...Args>
auto Test(Q elem, Args ...args){
    return elem + Test(args...);
}

template <typename Q>
void IncBasico(Q &elem){    elem += 1;     }

template <typename Q>
void Suma(Q &elem, Q p1){    elem += p1;     }

template <typename Q>
void Mult(Q &elem, Q p1){    elem *= p1;     }

#endif // __VARIADIC_UTIL__