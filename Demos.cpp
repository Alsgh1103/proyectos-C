#include <iostream>
#include "containers/binarytree.h"
#include "containers/binarytreeAVL.h"
#include "variadic-util.h"

using namespace std;

void DemoBinaryTree(){
    cout << endl << "CONSTRUYENDO ARBOL ASCENDENTE" << endl;
    CBinaryTree<TreeTraitAscending<T1>> arbol;
    
    arbol.Insert(50, 500);
    arbol.Insert(30, 300);
    arbol.Insert(70, 700);
    arbol.Insert(20, 200);
    arbol.Insert(40, 400);
    arbol.Insert(60, 600);
    arbol.Insert(80, 800);

    cout << "Arbol Inicial: " << endl << endl;
    arbol.Print();
    cout<< endl;

    cout << endl << "RECORRIDOS" << endl;
    cout << "PreOrder:  ";
    arbol.PreOrder(Imprimir<T1>, " ");
    cout << endl;

    cout << "InOrder:   ";
    arbol.InOrder(Imprimir<T1>, " ");
    cout << endl;

    cout << "PostOrder: ";
    arbol.PostOrder(Imprimir<T1>, " ");
    cout << endl;

    cout << endl << "FOREACH" << endl;
    cout << "Sumando 10 a todo... " << endl;
    arbol.Foreach(Suma<T1>, 10);
    arbol.InOrder(Imprimir<T1>, " ");
    cout << endl;

    cout << "Multiplicando por 2... " << endl;
    arbol.Foreach(Mult<T1>, 2);
    arbol.InOrder(Imprimir<T1>, " ");
    cout << endl;

    cout << endl << "FIRST THAT" << endl;
    cout<< "Encontrando al primer elemento mayor a 100..." << endl;
    auto itFound = arbol.FirstThat(EsMayorQue<T1>, 100);
    if (itFound != arbol.end()) 
        cout << "Elemento encontrado: " << *itFound << endl;
    
    cout << endl << "PERSISTENCIA Y MANEJO DE ARCHIVOS" << endl;

    cout << "Borrando raiz actual (120)..." << endl;
    arbol.Remove(120);
    arbol.Print();

    string nombreArchivo = "arbol_data.txt";
    cout << "Guardando en disco ('" << nombreArchivo << "')..." << endl;
    
    ofstream archivoSalida(nombreArchivo);
    if (archivoSalida.is_open()) {
        archivoSalida << arbol;
        archivoSalida.close();
        cout << "-> Guardado exitoso." << endl;
    } else {
        cerr << "Error al crear el archivo." << endl;
    }

    cout << "Leyendo de disco para reconstruir..." << endl;
    CBinaryTree<TreeTraitAscending<T1>> arbolLeido;
    
    ifstream archivoEntrada(nombreArchivo);
    if (archivoEntrada.is_open()) {
        archivoEntrada >> arbolLeido;
        archivoEntrada.close();
        cout << "-> Arbol recuperado (Visual): " << endl;
        arbolLeido.Print();
    } else {
        cerr << "No se pudo abrir el archivo para lectura." << endl;
    }

    cout << endl << "CONSTRUCTOR COPIA" << endl;
    CBinaryTree<TreeTraitAscending<T1>> arbolCopia(arbolLeido);
    cout << "Insertando 145 en la COPIA..." << endl;
    arbolCopia.Insert(145, 8999);
    cout << "Original   (Leido): " << arbolLeido << endl;
    cout << "Copia (Modificado): " << arbolCopia << endl;

    cout << endl << "MOVE CONSTRUCTOR" << endl;
    CBinaryTree<TreeTraitAscending<T1>> arbolMovido(move(arbolCopia));
    cout << "Estado 'arbolCopia' : " << arbolCopia  << endl;
    cout << "Estado 'arbolMovido': " << arbolMovido << endl;
    
    cout << endl << "PRUEBA AVL" << endl;
    CBinaryTree<TreeTraitDescending<T1>> arbolNormal;

    cout << "Insertando 10, 20, 30, 40, 50 en CBinaryTree normal..." << endl;
    arbolNormal.Insert(10, 1);
    arbolNormal.Insert(20, 2);
    arbolNormal.Insert(30, 3);
    arbolNormal.Insert(40, 4);
    arbolNormal.Insert(50, 5);

    cout << "ESTADO DEL ARBOL NORMAL (Degenerado en linea):" << endl << endl;
    arbolNormal.Print();

    cout << endl << "Copiando a CBinaryTreeAVL mediante constructor..." << endl;
    CBinaryTreeAVL<TreeTraitDescending<T1>> arbolAVL(arbolNormal);

    cout << "ESTADO DEL ARBOL AVL (Balanceado automaticamente):" << endl << endl;
    arbolAVL.Print();

    cout << endl << "Recorrido InOrder Descendente (ambos deben ser iguales):" << endl;
    cout << "Normal: "; arbolNormal.InOrder(Imprimir<T1>, " "); cout << endl;
    cout << "AVL:    "; arbolAVL.InOrder(Imprimir<T1>, " "); cout << endl;

    cout << endl << "Aplicando Foreach: Sumando 5 a todos los elementos..." << endl;
    arbolAVL.Foreach(Suma<T1>, 5);
    cout << "Valores actualizados:" << endl << endl;
    arbolAVL.Print();

    T1 limite = 30;
    cout << endl << "Aplicando FirstThat: Buscando el primero mayor a " << limite << "..." << endl;

    auto itEncontrado = arbolAVL.FirstThat(EsMayorQue<T1>, limite);

    if (itEncontrado != arbolAVL.end()) {
        cout << "Elemento encontrado: " << *itEncontrado << endl;
    } else {
        cout << "No se encontro ningun elemento mayor a " << limite << endl;
    }

    cout << endl << "Recorrido InOrder final del AVL: ";
    arbolAVL.InOrder(Imprimir<T1>, " ");
    cout << endl;
}