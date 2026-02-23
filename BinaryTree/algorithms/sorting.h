#ifndef __SORTING_H__
#define __SORTING_H__
#include "../util.h"
#include "../compareFunc.h"

// void BurbujaClasico(ContainerElemType* arr,
                    // ContainerRange n, CompFunc pComp);


// Bubble
template <typename T>
void BurbujaRecursivo(T arr[], ContainerRange n, 
                      bool (*pComp)(const T &, const T &) ) {
    if (n <= 1)
        return;
    for (auto j = 1; j < n; ++j)
        if ( pComp(arr[j], arr[0]) )
            intercambiar(arr[0], arr[j]);
    BurbujaRecursivo(arr+1, n-1, pComp);
}

void DemoBurbuja();

// ContainerRange  particionar(ContainerElemType* arr, ContainerRange first, ContainerRange last, CompFunc pComp);
// void QuickSort  (ContainerElemType* arr, ContainerRange first, ContainerRange last, CompFunc pComp);
// void DemoQuickSort();

// void Merge(ContainerElemType* arr, const ContainerRange left, const ContainerRange mid, const ContainerRange right, CompFunc pComp); 
// void MergeSort(ContainerElemType* arr, const ContainerRange begin, const ContainerRange end, CompFunc pComp);
// void DemoMergeSort();

void DemoSorting();
void DemoTemplates();

#endif // __SORTING_H__