#include <iostream>
#include <fstream>
#include <string>
#include "containers/AVL.h"

using namespace std;

void PrintIntAVL (T1&);
void PorDosAVL   (T1&);
bool EsMayor45AVL(T1&);
void PrintStrAVL (string&);
void PrintAscSquence();
void DemoAVL(){

    //Creación de AVL Ascending
    cout << "----------DEMO AVL-------------" << endl;
    cout << "Insercion en secuencia ascendente" << endl;
    PrintAscSquence();

    cout << endl;
    cout << "AVL 1 - Ascending" << endl;
    CAVL<TreeTraitAscending<T1>> AVL1;
    AVL1.Insert(50, 1);
    AVL1.Insert(30, 2);
    AVL1.Insert(70, 3);
    AVL1.Insert(20, 4);
    AVL1.Insert(40, 5);
    AVL1.Insert(60, 6);
    AVL1.Insert(80, 7);
    cout << AVL1 << endl;

    //Impresion visual del arbol
    cout << endl << "PrintTree" << endl;
    AVL1.PrintTree();

    //Probando recorridos
    cout << endl << "Inorden" << endl;
    AVL1.Inorden(PrintIntAVL);
    cout << endl;

    cout << endl << "Preorden" << endl;
    AVL1.Preorden(PrintIntAVL);
    cout << endl;

    cout << endl << "Postorden" << endl;
    AVL1.Postorden(PrintIntAVL);
    cout << endl;

    //Probando Foreach
    cout << endl << "Foreach (x2)" << endl;
    AVL1.Foreach(PorDosAVL);
    cout << endl;

    //Probando FirstThat
    cout << endl << "FirstThat (primer valor > 45)" << endl;
    T1* pFound = AVL1.FirstThat(EsMayor45AVL);
    if (pFound)
        cout << "Encontrado: " << *pFound << endl;
    else
        cout << "No encontrado" << endl;

    //Probando iteradores
    cout << endl << "ForwardIterator" << endl;
    for (auto it = AVL1.begin(); it != AVL1.end(); ++it)
        cout << *it << " ";
    cout << endl;

    cout << endl << "BackwardIterator" << endl;
    for (auto it = AVL1.rbegin(); it != AVL1.rend(); ++it)
        cout << *it << " ";
    cout << endl;

    //Probando Remove - nodo hoja
    cout << endl << "Remove - nodo hoja (20)" << endl;
    AVL1.Remove(20);
    cout << AVL1 << endl;
    AVL1.PrintTree();

    //Probando Remove - un hijo (ahora 30 queda con solo hijo derecho)
    cout << endl << "Remove - nodo un hijo (30)" << endl;
    AVL1.Remove(30);
    cout << AVL1 << endl;
    AVL1.PrintTree();

    //Probando Remove - dos hijos
    cout << endl << "Remove - nodo dos hijos (50)" << endl;
    AVL1.Remove(50);
    cout << AVL1 << endl;
    AVL1.PrintTree();

    //Probando que el AVL rebalancea al insertar en secuencia
    cout << endl << "AVL con insercion en secuencia (1..7)" << endl;
    CAVL<TreeTraitAscending<T1>> AVL2;
    AVL2.Insert(1, 1);
    AVL2.Insert(2, 2);
    AVL2.Insert(3, 3);
    AVL2.Insert(4, 4);
    AVL2.Insert(5, 5);
    AVL2.Insert(6, 6);
    AVL2.Insert(7, 7);
    cout << AVL2 << endl;
    AVL2.PrintTree();

    //Probando constructor copia
    cout << endl << "Copy constructor" << endl;
    cout << "AVL3" << endl;
    CAVL<TreeTraitAscending<T1>> AVL3 = AVL1;
    cout << AVL3 << endl;

    //Probando operador asignación '=' en copia
    CAVL<TreeTraitAscending<T1>> AVL4;
    AVL4.Insert(100, 10);
    AVL4.Insert(200, 20);
    cout << endl << "AVL4 antes de copiar" << endl << AVL4 << endl;

    AVL4 = AVL1;
    cout << "AVL4 despues de copiar de AVL1" << endl << AVL4 << endl;

    //Probando Move Constructor
    cout << endl << "AVL5 robando datos de AVL4" << endl;
    CAVL<TreeTraitAscending<T1>> AVL5 = std::move(AVL4);
    cout << AVL5 << endl;
    cout << "AVL4 actual" << endl;
    cout << AVL4 << endl;

    //Probando operador asignación '=' en move
    cout << endl << "AVL4 robando datos de AVL1" << endl;
    AVL4 = std::move(AVL1);
    cout << AVL4 << endl;
    cout << "AVL1 actual" << endl;
    cout << AVL1 << endl;

    //Probando AVL Descending
    cout << endl << "AVL Descending" << endl;
    CAVL<TreeTraitDescending<T1>> AVLDesc;
    AVLDesc.Insert(50, 1);
    AVLDesc.Insert(30, 2);
    AVLDesc.Insert(70, 3);
    AVLDesc.Insert(20, 4);
    AVLDesc.Insert(80, 5);
    cout << AVLDesc << endl;
    AVLDesc.PrintTree();

    //Probando con Strings
    cout << endl << "AVL con Strings (Ascending)" << endl;
    CAVL<TreeTraitAscending<string>> StringAVL;
    StringAVL.Insert("Zapato", 1);
    StringAVL.Insert("Arbol",  2);
    StringAVL.Insert("Mesa",   3);
    StringAVL.Insert("Casa",   4);
    StringAVL.Insert("Dedo",   5);
    StringAVL.Insert("Nube",   6);
    cout << StringAVL << endl;
    StringAVL.PrintTree();

    cout << endl << "Remove (Mesa)" << endl;
    StringAVL.Remove("Mesa");
    cout << StringAVL << endl;
    StringAVL.PrintTree();

    //Probando istream - operator>>
    ifstream file("BinaryTree.txt");
    CAVL<TreeTraitAscending<T1>> AVL6;
    file >> AVL6;
    file.close();
    cout << endl << "Lectura de archivo en Ascending" << endl;
    cout << AVL6 << endl;
    AVL6.PrintTree();
}

void PrintIntAVL (T1& val)     { cout << val << " "; }
void PorDosAVL   (T1& val)     { cout << val * 2 << " "; }
bool EsMayor45AVL(T1& val)     { return val > 45; }
void PrintStrAVL (string& val) { cout << val << " "; }
void PrintAscSquence(){
    CAVL<TreeTraitAscending<T1>> AVLasc;
    for (auto i=1; i < 10; ++i){
        cout << "Insercion " <<i << ":" << endl;
        AVLasc.Insert(i, i*10);
        AVLasc.PrintTree();
    }
}