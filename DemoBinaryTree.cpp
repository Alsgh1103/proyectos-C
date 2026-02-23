#include <iostream>
#include <fstream>
#include <string>
#include "containers/binarytree.h"

using namespace std;

void PrintInt   (T1&);
void PorDos     (T1&);
bool EsMayor45  (T1&);
void PrintStr   (string&);

void DemoBinaryTree(){

    //Creación de BinaryTree Ascending
    cout << "----------DEMO BINARY TREE-------------" << endl;
    cout << "BinaryTree 1 - Ascending" << endl;
    CBinaryTree<TreeTraitAscending<T1>> Tree1;
    Tree1.Insert(50, 1);
    Tree1.Insert(30, 2);
    Tree1.Insert(70, 3);
    Tree1.Insert(20, 4);
    Tree1.Insert(40, 5);
    Tree1.Insert(60, 6);
    Tree1.Insert(80, 7);
    cout << Tree1 << endl;

    //Probando recorridos
    cout << endl << "Inorden" << endl;
    Tree1.Inorden(PrintInt);
    cout << endl;

    cout << endl << "Preorden" << endl;
    Tree1.Preorden(PrintInt);
    cout << endl;

    cout << endl << "Postorden" << endl;
    Tree1.Postorden(PrintInt);
    cout << endl;

    //Probando Foreach
    cout << endl << "Foreach (x2)" << endl;
    Tree1.Foreach(PorDos);
    cout << endl;

    //Probando FirstThat
    cout << endl << "FirstThat (primer valor > 45)" << endl;
    T1* pFound = Tree1.FirstThat(EsMayor45);
    if (pFound)
        cout << "Encontrado: " << *pFound << endl;
    else
        cout << "No encontrado" << endl;


    //Probando iteradores
    cout << endl << "ForwardIterator" << endl;
    for (auto it = Tree1.begin(); it != Tree1.end(); ++it)
        cout << *it << " ";
    cout << endl;

    cout << endl << "BackwardIterator" << endl;
    for (auto it = Tree1.rbegin(); it != Tree1.rend(); ++it)
        cout << *it << " ";
    cout << endl;

    //Probando Remove
    cout << endl << "Remove - nodo hoja (20)" << endl;
    Tree1.Remove(20);
    cout << Tree1 << endl;

    cout << endl << "Remove - nodo un hijo (30)" << endl;
    Tree1.Remove(30);
    cout << Tree1 << endl;

    cout << endl << "Remove - nodo dos hijos (50)" << endl;
    Tree1.Remove(50);
    cout << Tree1 << endl;

    //Probando constructor copia
    cout << endl << "Copy constructor" << endl;
    cout << "Tree2" << endl;
    CBinaryTree<TreeTraitAscending<T1>> Tree2 = Tree1;
    cout << Tree2 << endl;

    //Probando operador asignación '=' en copia
    CBinaryTree<TreeTraitAscending<T1>> Tree3;
    Tree3.Insert(100, 10);
    Tree3.Insert(200, 20);
    cout << endl << "Tree3 antes de copiar" << endl << Tree3 << endl;

    Tree3 = Tree1;
    cout << "Tree3 despues de copiar de Tree1" << endl << Tree3 << endl;

    //Probando Move Constructor
    cout << endl << "Tree4 robando datos de Tree3" << endl;
    CBinaryTree<TreeTraitAscending<T1>> Tree4 = std::move(Tree3);
    cout << Tree4 << endl;
    cout << "Tree3 actual" << endl;
    cout << Tree3 << endl;

    //Probando operador asignación '=' en move
    cout << endl << "Tree3 robando datos de Tree1" << endl;
    Tree3 = std::move(Tree1);
    cout << Tree3 << endl;
    cout << "Tree1 actual" << endl;
    cout << Tree1 << endl;

    //Probando BinaryTree Descending
    cout << endl << "BinaryTree Descending" << endl;
    CBinaryTree<TreeTraitDescending<T1>> TreeDesc;
    TreeDesc.Insert(50, 1);
    TreeDesc.Insert(30, 2);
    TreeDesc.Insert(70, 3);
    TreeDesc.Insert(20, 4);
    TreeDesc.Insert(80, 5);
    cout << TreeDesc << endl;

    //Probando con Strings
    cout << endl << "BinaryTree con Strings (Ascending)" << endl;
    CBinaryTree<TreeTraitAscending<string>> StringTree;
    StringTree.Insert("Zapato", 1);
    StringTree.Insert("Arbol",  2);
    StringTree.Insert("Mesa",   3);
    StringTree.Insert("Casa",   4);
    StringTree.Insert("Dedo",   5);
    StringTree.Insert("Nube",   6);
    cout << StringTree << endl;

    cout << endl << "Remove (Mesa)" << endl;
    StringTree.Remove("Mesa");
    cout << StringTree << endl;

    //Probando istream - operator>>
    ifstream file("BinaryTree.txt");
    CBinaryTree<TreeTraitAscending<T1>> Tree5;
    file >> Tree5;
    file.close();
    cout << endl << "Lectura de archivo en Ascending" << endl;
    cout << Tree5 << endl;
}

void PrintInt   (T1& val)     { cout << val << " "; }
void PorDos     (T1& val)     { cout << val * 2 << " "; }
bool EsMayor45  (T1& val)     { return val > 45; }
void PrintStr   (string& val) { cout << val << " "; }

