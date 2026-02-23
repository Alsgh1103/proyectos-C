#include <iostream>
#include <string>
#include "pointers.h"
using namespace std;

void print(string str, int val){
    cout << str << val << endl;
}

void f1(int n ){    ++n;   }
void f2(int &n){    ++n;   }
void f3(int *pi){   ++*pi;   pi = nullptr;  }
void f4(int *&rpi){   ++*rpi;   rpi = nullptr;  }

void DemoPointers1(){
    int x = 10, &ri = x; // b=20,
    int *p1 = nullptr;  //, *q = nullptr, *ri;
    int **pp = nullptr;

    p1 = &x;
    pp = &p1;

    print("Before f1, x = ", x);
    f1(x);
    f1(5);
    f1(x+7);
    f1(ri);
    print("After  f1, x = ", x);
    
    x = 10;
    print("Before f2, x = ", x);
    f2(x);
    // f2(35);
    // f2(x+4);
    f2(*p1);
    f2(**pp);
    print("After  f2, x = ", x);


    x = 10; p1 = &x;
    // b = 20; //q = &b;
    print("Before f3, x = ", x);
    f3(&x);
    f3(&ri);
    f3(p1);
    f3(*pp);
    print("After  f3, x = ", x);

    x = 10; p1 = &x;
    // b = 20; //q = &b;
    print("Before f4, x = ", x);
    // f4(&x);
    f4(*pp); // p1 = nullptr
    p1 = &x; // reset p1
    f4(p1);
    print("After  f4, x = ", x);
}