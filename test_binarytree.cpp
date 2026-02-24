#include <iostream>
#include <type_traits>
#include <cassert>
#include <thread>
#include <vector>

// 1. INCLUSIÓN OBLIGATORIA
// Si el alumno no creó este archivo, la compilación de GitHub fallará aquí mismo.
#include "containers/binarytree.h"
#include "containers/AVL.h"
#if __has_include("containers/binarytreeAVL.h")
    #include "containers/binarytreeAVL.h"
#elif __has_include("containers/AVL.h")
    #include "containers/AVL.h"
#else
    // Si el alumno no subió ninguno de los dos, forzamos un error claro para GitHub Actions
    #error "ERROR ALUMNO: No se encontro el archivo del AVL. Debe llamarse 'binarytreeAVL.h' o 'AVL.h' y estar en la carpeta 'containers'"
#endif

// --- CONFIGURACIÓN DEL ENTORNO DE PRUEBA ---
// Usaremos el trait Ascendente que ya tienes implementado
using TestTraits = TreeTraitAscending<int>;
using TreeType = CBinaryTree<TestTraits>;
using AVLTreeType = CBinaryTreeAVL<TestTraits>;

// --- 2. PRUEBAS ESTÁTICAS (Static Assertions) ---

// A. Verificación de Arquitectura (Herencia del AVL)
static_assert(std::is_base_of_v<TreeType, AVLTreeType>, 
              "ERROR: La clase CBinaryTreeAVL DEBE heredar de CBinaryTree");

// B. Verificación de Regla de los 5 (Constructores y Destructor Safe)
static_assert(std::is_copy_constructible_v<TreeType>, 
              "ERROR: CBinaryTree debe tener un Constructor Copia");
static_assert(std::is_move_constructible_v<TreeType>, 
              "ERROR: CBinaryTree debe tener un Move Constructor");
static_assert(std::has_virtual_destructor_v<TreeType>, 
              "ERROR: El Destructor de CBinaryTree debe ser 'virtual' para evitar memory leaks");

// C. Verificación SFINAE para Sobrecarga de Operadores (<< y >>)
template <typename T, typename = void> struct has_ostream_op : std::false_type {};
template <typename T>
struct has_ostream_op<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T&>())>> : std::true_type {};

template <typename T, typename = void> struct has_istream_op : std::false_type {};
template <typename T>
struct has_istream_op<T, std::void_t<decltype(std::declval<std::istream&>() >> std::declval<T&>())>> : std::true_type {};

static_assert(has_ostream_op<TreeType>::value, "ERROR: No se sobrecargó el operador <<");
static_assert(has_istream_op<TreeType>::value, "ERROR: No se sobrecargó el operador >>");

// --- 3. PRUEBAS DINÁMICAS (Ejecución) ---

void TestIterators() {
    std::cout << "[Test] Verificando Forward y Backward Iterators en BinaryTree..." << std::endl;
    TreeType tree;
    tree.Insert(20, 1);
    tree.Insert(10, 2);
    tree.Insert(30, 3);

    // Prueba de Forward Iterator
    auto it = tree.begin();
    assert(it != tree.end() && "El iterador forward no debe estar vacio");
    assert(*it == 10 && "El iterador forward (ascendente) debe empezar en 10");

    // Prueba de Backward Iterator
    auto rit = tree.rbegin();
    assert(rit != tree.rend() && "El iterador backward no debe estar vacio");
    assert(*rit == 30 && "El iterador backward (ascendente) debe empezar en el mayor (30)");

    std::cout << "  ✓ Iteradores direccionales pasaron la prueba." << std::endl;
}

void TestVariadicTemplates() {
    std::cout << "[Test] Verificando Foreach y FirstThat (Variadic Templates)..." << std::endl;
    TreeType tree;
    tree.Insert(15, 1);
    tree.Insert(25, 2);

    int suma_test = 0;

    // Prueba Foreach (Pasando la 'suma_test' como argumento variádico extra)
    tree.Foreach([](int& val, int& acumulador) {
        acumulador += val;
    }, suma_test);

    assert(suma_test == 40 && "El Foreach no sumó correctamente los elementos");

    // Prueba FirstThat (Pasando un 'limite' de 20 como argumento variádico)
    auto found = tree.FirstThat([](int& val, int limite) {
        return val > limite;
    }, 20);

    assert(found != tree.end() && *found == 25 && "FirstThat no encontró el elemento correcto");
    std::cout << "  ✓ Variadic Templates pasaron la prueba." << std::endl;
}

void TestConcurrencyAVL() {
    std::cout << "[Test] Verificando control de concurrencia (Mutex) en el AVL..." << std::endl;
    AVLTreeType avl_tree; 

    // Prueba de estrés: 3 hilos insertando datos al mismo tiempo en el AVL
    auto insert_job = [&avl_tree](int start, int count) {
        for (int i = start; i < start + count; ++i) {
            avl_tree.Insert(i, i * 10);
        }
    };

    std::thread t1(insert_job, 1, 100);
    std::thread t2(insert_job, 101, 100);
    std::thread t3(insert_job, 201, 100);

    t1.join();
    t2.join();
    t3.join();

    // Verificamos que no se haya perdido ningún nodo por Race Conditions
    int count = 0;
    for(auto it = avl_tree.begin(); it != avl_tree.end(); ++it) {
        count++;
    }
    
    assert(count == 300 && "Fallo en la concurrencia: Faltan nodos o hubo corrupción de memoria");
    std::cout << "  ✓ Mutex y Concurrencia superaron la prueba de estrés en AVL." << std::endl;
}

int main() {
    std::cout << "=== INICIANDO BATERÍA DE PRUEBAS DEL BINARY TREE & AVL ===" << std::endl;
    
    TestIterators();
    TestVariadicTemplates();
    TestConcurrencyAVL();

    std::cout << "=== TODAS LAS PRUEBAS FUERON EXITOSAS ===" << std::endl;
    return 0; // Check verde en GitHub
}
