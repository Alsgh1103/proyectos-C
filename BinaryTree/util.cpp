#include <iostream>
#include "util.h"

TT fact(T n) {
    if (n <= 1) 
        return 1L;
    return n * fact(n - 1);
}

void DemoUtil(){
    T n = 5;
    TT result = fact(n);
    std::cout << "Factorial of " << n << " is " << result << std::endl;
}
