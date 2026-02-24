#include <iostream>
#include <type_traits>
#include <cassert>
#include <vector>
#include <mutex>

// --- PARCHE DE COMPILACIÓN PARA ALUMNOS ---
// Si algún alumno olvidó poner "std::" en sus archivos (como en el ostream o mutex),
// poner esto antes de sus includes salva su código y permite que la prueba continúe.
using namespace std;

#include "containers/binarytree.h"

// Buscamos el AVL con el nombre que el alumno le haya puesto
#if __has_include("containers/binarytreeAVL.h")
    #include "containers/binarytreeAVL.h"
#elif __has_include("containers/AVL.h")
    #include "containers/AVL.h"
#endif

// Configuramos el entorno usando el Trait del alumno
using TestTraits = TreeTraitAscending<int>;
using TreeType = CBinaryTree<TestTraits>;

// =========================================================================
// SFINAE DETECTORS (Detectores Universales de Requisitos)
// No importa el tipo de retorno exacto, solo importa que el método EXISTA y funcione.
// =========================================================================

// 1. Operadores I/O
template <typename T, typename = void> struct has_ostream : std::false_type {};
template <typename T> struct has_ostream<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T&>())>> : std::true_type {};

template <typename T, typename = void> struct has_istream : std::false_type {};
template <typename T> struct has_istream<T, std::void_t<decltype(std::declval<std::istream&>() >> std::declval<T&>())>> : std::true_type {};

// 2. Insert y Remove
template <typename T, typename = void> struct has_insert : std::false_type {};
template <typename T> struct has_insert<T, std::void_t<decltype(std::declval<T>().Insert(1, 1))>> : std::true_type {};

template <typename T, typename = void> struct has_remove : std::false_type {};
template <typename T> struct has_remove<T, std::void_t<decltype(std::declval<T>().Remove(1))>> : std::true_type {};

// 3. Iteradores (Comprobamos que tengan begin() y rbegin())
template <typename T, typename = void> struct has_forward_it : std::false_type {};
template <typename T> struct has_forward_it<T, std::void_t<decltype(std::declval<T>().begin())>> : std::true_type {};

template <typename T, typename = void> struct has_backward_it : std::false_type {};
template <typename T> struct has_backward_it<T, std::void_t<decltype(std::declval<T>().rbegin())>> : std::true_type {};

// 4. Recorridos (Flexibilidad: Acepta que lo llamen 'InOrder' o 'Inorden' y debe ser variádico)
template <typename T, typename = void> struct has_inorder : std::false_type {};
template <typename T> struct has_inorder<T, std::void_t<decltype(std::declval<T>().InOrder([](int&, int){}, 1))>> : std::true_type {};

template <typename T, typename = void> struct has_inorden : std::false_type {};
template <typename T> struct has_inorden<T, std::void_t<decltype(std::declval<T>().Inorden([](int&, int){}, 1))>> : std::true_type {};

// 5. Foreach y FirstThat (Variádicos)
template <typename T, typename = void> struct has_foreach : std::false_type {};
template <typename T> struct has_foreach<T, std::void_t<decltype(std::declval<T>().Foreach([](int&, int){}, 1))>> : std::true_type {};

template <typename T, typename = void> struct has_firstthat : std::false_type {};
template <typename T> struct has_firstthat<T, std::void_t<decltype(std::declval<T>().FirstThat([](int&, int){return true;}, 1))>> : std::true_type {};


int main() {
    std::cout << "[INFO] Evaluando estrictamente los requisitos de FISI..." << std::endl;

    // --- 1. REGLA DE LOS 5 (Constructores y Destructor) ---
    static_assert(std::is_copy_constructible_v<TreeType>, "ERROR: Falta Constructor Copia");
    static_assert(std::is_move_constructible_v<TreeType>, "ERROR: Falta Move Constructor (o no usa exchange)");
    static_assert(std::has_virtual_destructor_v<TreeType>, "ERROR: Falta Destructor Virtual (Safe Destructor)");

    // --- 2. OPERACIONES BÁSICAS ---
    static_assert(has_insert<TreeType>::value, "ERROR: Falta el metodo Insert");
    static_assert(has_remove<TreeType>::value, "ERROR: Falta el metodo Remove");

    // --- 3. ITERADORES ---
    static_assert(has_forward_it<TreeType>::value, "ERROR: Falta Forward Iterator (metodo begin)");
    static_assert(has_backward_it<TreeType>::value, "ERROR: Falta Backward Iterator (metodo rbegin)");

    // --- 4. RECORRIDOS ---
    static_assert(has_inorder<TreeType>::value || has_inorden<TreeType>::value, "ERROR: Falta recorrido InOrder / Inorden");

    // --- 5. VARIADIC TEMPLATES ---
    static_assert(has_foreach<TreeType>::value, "ERROR: Falta metodo Foreach (Variadic Template)");
    static_assert(has_firstthat<TreeType>::value, "ERROR: Falta metodo FirstThat (Variadic Template)");

    // --- 6. OPERADORES I/O ---
    static_assert(has_ostream<TreeType>::value, "ERROR: Falta sobrecarga de operator<<");
    static_assert(has_istream<TreeType>::value, "ERROR: Falta sobrecarga de operator>>");

    std::cout << "✅ Todos los requerimientos fueron cumplidos exitosamente." << std::endl;
    return 0;
<<<<<<< HEAD
}
=======
}
>>>>>>> b5af268c5fec1bb5f1d9c0202bdc7100e849c03a
