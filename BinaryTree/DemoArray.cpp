#include <assert.h>
#include <iostream>
#include <fstream>
using namespace std;
#include "containers/array.h"
#include "compareFunc.h"
#include "variadic-util.h"

using T1 = int;

bool Mult7(T1 &elem){
  return elem%7 == 0;
}

template <typename Q>
void Print(Q &elem){    cout << elem << ",";     }

void DemoArray() {
    const int N = 5;
    //  /using 
    CArray< Trait1<T1> > arr1(N);
    
    for (auto i = 0; i < N; ++i)
      arr1.push_back(i * 10, i);
    cout << "First part finished" << endl;
    arr1[3] = 49;
    arr1.push_back(80, 16);
    arr1.push_back(75, 13);

    // test for [] operator with out of bounds index
    arr1[9] = 10;
    arr1[12] = 15;

    cout << "Ready to print" << endl;
    cout << arr1 << endl;

    ofstream of("array.txt");
    of << arr1;
    of.close();
    cout << "DemoArray finished." << endl;

    arr1.sort(&Mayor);
    cout << arr1 << endl;

    arr1.sort(&Menor);
    cout << arr1 << endl;

    arr1.Foreach(&Suma<T1>, 4);
    cout << arr1 << endl;

    arr1.Foreach(&Mult<T1>, 3);
    cout << arr1 << endl;

    cout << "Antes del Foreach externo, getSize()=" << arr1.getSize() << endl;
    Foreach(arr1, &IncBasico<T1>);
    cout << arr1 << endl;
    cout << "Terminó #1" << endl;

    ::Foreach(arr1, &Print<T1>);                      cout << endl;
    ::Foreach(arr1.begin(),  arr1.end(),  &Print<T1>);  cout << endl;
    ::Foreach(arr1.rbegin(), arr1.rend(), &Print<T1>);  cout << endl;

    cout << "Terminó #2" << endl; 

    auto iter = arr1.FirstThat( &Mult7 ); 
    if( iter != arr1.end() )
    {   cout << "El primer multiplo de 7 es: " << *iter << endl; }

    // using T2 = string;
    // CArray< Trait1<T2> > arr2(N);
    // arr2[2] = "UNMSM";
    // arr2[4] = "FC";
    // cout << arr2 <<endl;
    // arr2.Foreach(&Suma<T2>, "Enero");
    // cout << arr2 <<endl;

    // int x = Test(5, 3, 2);
    // cout << x << endl;
    // auto r = Test(1, 2, 3.4, 4, 5, 6, 7);
    // cout << r << endl;

    // auto str = Test(string("Hola"), string("FC"), string("UNMSM") );
    // cout << str << endl;
}

