// ============================================================
//  test_binarytree.cpp
//  Batería de pruebas unitarias generales para CBinaryTree
//  Requerimientos evaluados:
//    1) Constructor Copia, Move Constructor (exchange), Destructor virtual
//    2) Insert y Remove
//    3) Forward Iterator y Backward Iterator
//    4) PreOrder, InOrder, PostOrder
//    5) Foreach y FirstThat con variadic templates
//    6) operator<< y operator>>
//    7) Mutex (prueba de concurrencia)
// ============================================================

#include <iostream>
#include <sstream>
#include <cassert>
#include <thread>
#include <vector>
#include <type_traits>
#include <algorithm>

// --- INCLUSIÓN OBLIGATORIA ---
// Si el alumno no creó este archivo, la compilación fallará aquí.
#include "containers/binarytree.h"

// ---------------------------------------------------------------------------
// Tipo de árbol usado en todas las pruebas.
// TreeTraitAscending<int>: int con orden ascendente.
// ---------------------------------------------------------------------------
using TestTraits = TreeTraitAscending<int>;
using TreeType   = CBinaryTree<TestTraits>;

// ============================================================
// SECCIÓN 1 – PRUEBAS ESTÁTICAS (tiempo de compilación)
// ============================================================

// 1a. Constructor Copia
static_assert(std::is_copy_constructible_v<TreeType>,
    "REQUERIMIENTO 1: CBinaryTree debe tener Constructor Copia");

// 1b. Move Constructor
static_assert(std::is_move_constructible_v<TreeType>,
    "REQUERIMIENTO 1: CBinaryTree debe tener Move Constructor");

// 1c. Destructor virtual (Safe Destructor)
static_assert(std::has_virtual_destructor_v<TreeType>,
    "REQUERIMIENTO 1: El destructor de CBinaryTree debe ser 'virtual'");

// 6a. SFINAE – operator<<
template <typename T, typename = void>
struct has_ostream_op : std::false_type {};
template <typename T>
struct has_ostream_op<T,
    std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T&>())>>
    : std::true_type {};

// 6b. SFINAE – operator>>
template <typename T, typename = void>
struct has_istream_op : std::false_type {};
template <typename T>
struct has_istream_op<T,
    std::void_t<decltype(std::declval<std::istream&>() >> std::declval<T&>())>>
    : std::true_type {};

static_assert(has_ostream_op<TreeType>::value,
    "REQUERIMIENTO 6: No se encontro operator<< para CBinaryTree");
static_assert(has_istream_op<TreeType>::value,
    "REQUERIMIENTO 6: No se encontro operator>> para CBinaryTree");

// ============================================================
// Helpers
// ============================================================
static void pass(const char* name) {
    std::cout << "  [PASS] " << name << "\n";
}
static void section(const char* name) {
    std::cout << "\n--- " << name << " ---\n";
}

// Cuenta cuántos nodos hay en el árbol usando el forward iterator
static int count_nodes(TreeType& tree) {
    int n = 0;
    for (auto it = tree.begin(); it != tree.end(); ++it)
        ++n;
    return n;
}

// Devuelve true si el arbol no tiene ningun elemento
static bool tree_is_empty(TreeType& tree) {
    return !(tree.begin() != tree.end());
}

// Devuelve true si el iterador es el end (usando solo operator!=)
static bool iter_at_end(TreeType::forward_iterator it, TreeType& tree) {
    return !(it != tree.end());
}

// ============================================================
// SECCIÓN 1b – Constructor Copia y Move Constructor (dinámico)
// ============================================================
void TestCopyAndMove() {
    section("REQUERIMIENTO 1: Constructor Copia y Move Constructor");

    // --- Constructor Copia ---
    TreeType original;
    original.Insert(10, 1);
    original.Insert(20, 2);
    original.Insert(30, 3);

    TreeType copy(original);

    // La copia debe tener los mismos elementos en el mismo orden (InOrder)
    std::vector<int> orig_vals, copy_vals;
    for (auto it = original.begin(); it != original.end(); ++it) orig_vals.push_back(*it);
    for (auto it = copy.begin();     it != copy.end();     ++it) copy_vals.push_back(*it);
    assert(orig_vals == copy_vals &&
        "Constructor Copia: la copia debe tener los mismos elementos");
    pass("Constructor Copia: la copia es identica al original");

    // Modificar la copia NO debe afectar al original (deep copy)
    copy.Insert(99, 9);
    std::vector<int> orig_after;
    for (auto it = original.begin(); it != original.end(); ++it) orig_after.push_back(*it);
    assert(orig_after == orig_vals &&
        "Constructor Copia: debe ser deep copy (modificar copia no toca original)");
    pass("Constructor Copia: deep copy confirmada");

    // --- Move Constructor ---
    TreeType src;
    src.Insert(100, 1);
    src.Insert(200, 2);
    src.Insert(300, 3);

    std::vector<int> src_before;
    for (auto it = src.begin(); it != src.end(); ++it) src_before.push_back(*it);

    TreeType moved(std::move(src));

    std::vector<int> moved_vals;
    for (auto it = moved.begin(); it != moved.end(); ++it) moved_vals.push_back(*it);

    assert(src_before == moved_vals &&
        "Move Constructor: el destino debe tener los elementos originales");
    // Tras el move con exchange, el origen queda vacio
    assert(tree_is_empty(src) &&
        "Move Constructor (exchange): el origen debe quedar vacio tras el move");
    pass("Move Constructor (exchange): transfiere propiedad y vacia el origen");
}

// ============================================================
// SECCIÓN 2 – Insert y Remove
// ============================================================
void TestInsertRemove() {
    section("REQUERIMIENTO 2: Insert y Remove");

    // --- Insert basico ---
    TreeType tree;
    tree.Insert(50, 1);
    tree.Insert(30, 2);
    tree.Insert(70, 3);
    tree.Insert(20, 4);
    tree.Insert(40, 5);

    assert(count_nodes(tree) == 5 &&
        "Insert: debe haber 5 nodos tras 5 inserciones");
    pass("Insert: 5 nodos insertados correctamente");

    // Insertar duplicado no debe crashear
    tree.Insert(50, 99);
    pass("Insert: insertar duplicado no provoca crash");

    // --- Remove hoja ---
    TreeType t2;
    t2.Insert(50, 1);
    t2.Insert(30, 2);
    t2.Insert(70, 3);
    t2.Remove(30);

    bool found30 = false;
    for (auto it = t2.begin(); it != t2.end(); ++it)
        if (*it == 30) found30 = true;
    assert(count_nodes(t2) == 2 && "Remove hoja: deben quedar 2 nodos");
    assert(!found30              && "Remove hoja: el nodo eliminado no debe aparecer");
    pass("Remove: eliminar nodo hoja");

    // --- Remove nodo con dos hijos ---
    TreeType t3;
    t3.Insert(50, 1);
    t3.Insert(30, 2);
    t3.Insert(70, 3);
    t3.Insert(20, 4);
    t3.Insert(40, 5);
    t3.Remove(30); // nodo interior con hijos 20 y 40

    bool found30b = false;
    for (auto it = t3.begin(); it != t3.end(); ++it)
        if (*it == 30) found30b = true;
    assert(count_nodes(t3) == 4 && "Remove nodo-2-hijos: deben quedar 4 nodos");
    assert(!found30b              && "Remove nodo-2-hijos: el nodo eliminado no debe aparecer");
    pass("Remove: eliminar nodo con dos hijos");

    // --- Remove elemento inexistente ---
    t3.Remove(999);
    pass("Remove: eliminar elemento inexistente no provoca crash");
}

// ============================================================
// SECCIÓN 3 – Forward Iterator y Backward Iterator
// ============================================================
void TestIterators() {
    section("REQUERIMIENTO 3: Forward Iterator y Backward Iterator");

    TreeType tree;
    tree.Insert(50, 1);
    tree.Insert(20, 2);
    tree.Insert(80, 3);
    tree.Insert(10, 4);
    tree.Insert(30, 5);

    // Forward: orden ASCENDENTE (10,20,30,50,80)
    std::vector<int> fwd;
    for (auto it = tree.begin(); it != tree.end(); ++it)
        fwd.push_back(*it);

    assert(fwd.size() == 5     && "Forward: debe visitar 5 nodos");
    assert(fwd[0]     == 10    && "Forward: primer elemento = minimo (10)");
    assert(fwd.back() == 80    && "Forward: ultimo elemento = maximo (80)");
    bool ascending = true;
    for (size_t i = 1; i < fwd.size(); ++i)
        if (fwd[i] < fwd[i-1]) ascending = false;
    assert(ascending           && "Forward: recorrido debe ser ascendente");
    pass("Forward Iterator: recorre en orden ascendente");

    // Backward: orden DESCENDENTE (80,50,30,20,10)
    std::vector<int> bwd;
    for (auto it = tree.rbegin(); it != tree.rend(); ++it)
        bwd.push_back(*it);

    assert(bwd.size() == 5     && "Backward: debe visitar 5 nodos");
    assert(bwd[0]     == 80    && "Backward: primer elemento = maximo (80)");
    assert(bwd.back() == 10    && "Backward: ultimo elemento = minimo (10)");
    bool descending = true;
    for (size_t i = 1; i < bwd.size(); ++i)
        if (bwd[i] > bwd[i-1]) descending = false;
    assert(descending          && "Backward: recorrido debe ser descendente");
    pass("Backward Iterator: recorre en orden descendente");

    // Arbol vacio: begin() debe ser end() (verificado con !=)
    TreeType empty;
    assert(tree_is_empty(empty) && "Arbol vacio: begin() debe igualar end()");
    pass("Iteradores en arbol vacio: se comportan correctamente");
}

// ============================================================
// SECCIÓN 4 – PreOrder, InOrder, PostOrder
// ============================================================
void TestTraversals() {
    section("REQUERIMIENTO 4: PreOrder, InOrder, PostOrder");

    //      50
    //     /  \
    //   30    70
    //   / \
    // 20  40
    TreeType tree;
    tree.Insert(50, 1);
    tree.Insert(30, 2);
    tree.Insert(70, 3);
    tree.Insert(20, 4);
    tree.Insert(40, 5);

    // InOrder: El lambda captura `inorder` por referencia porque los args
    // variadicos se pasan por valor internamente; la captura es la unica
    // forma de acumular resultados de forma fiable.
    std::vector<int> inorder;
    tree.InOrder([&inorder](int& v){ inorder.push_back(v); });
    assert(inorder.size() == 5 && "InOrder: debe visitar 5 nodos");
    bool in_asc = true;
    for (size_t i = 1; i < inorder.size(); ++i)
        if (inorder[i] < inorder[i-1]) in_asc = false;
    assert(in_asc && "InOrder: resultado debe estar en orden ascendente");
    pass("InOrder: recorre todos los nodos en orden ascendente");

    // PreOrder: raiz visitada PRIMERO
    std::vector<int> preorder;
    tree.PreOrder([&preorder](int& v){ preorder.push_back(v); });
    assert(preorder.size() == 5 && "PreOrder: debe visitar 5 nodos");
    assert(preorder[0] == 50    && "PreOrder: el primer nodo debe ser la raiz");
    pass("PreOrder: la raiz es visitada primero");

    // PostOrder: raiz visitada AL FINAL
    std::vector<int> postorder;
    tree.PostOrder([&postorder](int& v){ postorder.push_back(v); });
    assert(postorder.size() == 5 && "PostOrder: debe visitar 5 nodos");
    assert(postorder.back() == 50 && "PostOrder: el ultimo nodo debe ser la raiz");
    pass("PostOrder: la raiz es visitada al final");

    // Los tres recorridos deben visitar el mismo conjunto de nodos
    std::vector<int> s_in = inorder, s_pre = preorder, s_post = postorder;
    std::sort(s_in.begin(),   s_in.end());
    std::sort(s_pre.begin(),  s_pre.end());
    std::sort(s_post.begin(), s_post.end());
    assert(s_in == s_pre  && "Los tres recorridos deben visitar los mismos nodos");
    assert(s_in == s_post && "Los tres recorridos deben visitar los mismos nodos");
    pass("PreOrder, InOrder, PostOrder: visitan el mismo conjunto de nodos");
}

// ============================================================
// SECCIÓN 5 – Foreach y FirstThat con Variadic Templates
// ============================================================
void TestVariadicTemplates() {
    section("REQUERIMIENTO 5: Foreach y FirstThat (Variadic Templates)");

    TreeType tree;
    tree.Insert(10, 1);
    tree.Insert(20, 2);
    tree.Insert(30, 3);
    tree.Insert(40, 4);
    tree.Insert(50, 5);

    // --- Foreach: contar nodos visitados ---
    // El lambda captura `visited` por referencia (los args variadicos se
    // pasan por valor internamente, asi que la captura es la forma correcta).
    int visited = 0;
    tree.Foreach([&visited](int& /*val*/){ ++visited; });
    assert(visited == 5 && "Foreach: debe visitar exactamente 5 nodos");
    pass("Foreach: visita todos los nodos");

    // --- Foreach con argumento variádico extra (ejercita el path variádico) ---
    // El lambda captura suma por referencia; el arg variádico `limit` se
    // usa solo para verificar que el compilador acepta el argumento extra.
    int suma = 0;
    tree.Foreach([&suma](int& val, int limit){
        if (val > limit) suma += val;
    }, 0 /*limit=0: suma todo*/);
    assert(suma == 150 && "Foreach: la suma de 10+20+30+40+50 debe ser 150");
    pass("Foreach: acumula correctamente con argumento variadico extra");

    // --- FirstThat: primer elemento > 25 ---
    auto it = tree.FirstThat([](int& val, int lim){ return val > lim; }, 25);
    assert((it != tree.end()) && "FirstThat: debe encontrar un elemento > 25");
    assert(*it > 25            && "FirstThat: el elemento encontrado debe ser > 25");
    pass("FirstThat: encuentra el primer elemento que cumple el predicado");

    // --- FirstThat: predicado que nunca se cumple → end() ---
    auto it2 = tree.FirstThat([](int& val, int lim){ return val > lim; }, 9999);
    assert(iter_at_end(it2, tree) && "FirstThat: debe retornar end() si ningun nodo cumple");
    pass("FirstThat: retorna end() cuando ningun elemento cumple");

    // --- FirstThat con dos argumentos variadicos (rango [lo, hi]) ---
    auto it3 = tree.FirstThat([](int& val, int lo, int hi){
        return val > lo && val < hi;
    }, 15, 35);
    assert((it3 != tree.end()) && "FirstThat (2 args): debe encontrar elemento en rango");
    assert(*it3 > 15 && *it3 < 35 && "FirstThat (2 args): el valor hallado debe estar en [15,35]");
    pass("FirstThat: funciona con multiples argumentos variadicos");
}

// ============================================================
// SECCIÓN 6 – operator<< y operator>>
// ============================================================
void TestStreamOperators() {
    section("REQUERIMIENTO 6: operator<< y operator>>");

    TreeType tree;
    tree.Insert(50, 1);
    tree.Insert(20, 2);
    tree.Insert(80, 3);

    // --- operator<<: serializar ---
    std::ostringstream oss;
    oss << tree;
    std::string serialized = oss.str();
    assert(!serialized.empty() && "operator<<: la salida no debe estar vacia");
    assert(serialized.find("50") != std::string::npos && "operator<<: debe contener '50'");
    assert(serialized.find("20") != std::string::npos && "operator<<: debe contener '20'");
    assert(serialized.find("80") != std::string::npos && "operator<<: debe contener '80'");
    pass("operator<<: serializa correctamente los nodos");

    // --- operator>>: deserializar y comparar ---
    TreeType tree2;
    std::istringstream iss(serialized);
    iss >> tree2;

    std::vector<int> original_vals, restored_vals;
    for (auto it = tree.begin();  it != tree.end();  ++it) original_vals.push_back(*it);
    for (auto it = tree2.begin(); it != tree2.end(); ++it) restored_vals.push_back(*it);

    assert(original_vals == restored_vals &&
        "operator>>: el arbol restaurado debe tener los mismos elementos");
    pass("operator>>: deserializa y reconstruye el arbol correctamente");
}

// ============================================================
// SECCIÓN 7 – Mutex / Concurrencia
// ============================================================
void TestConcurrency() {
    section("REQUERIMIENTO 7: Mutex y seguridad en concurrencia");

    TreeType tree;
    const int THREADS      = 4;
    const int EACH         = 50;
    const int TOTAL        = THREADS * EACH; // 200

    // Cada hilo inserta un rango distinto sin solapamiento
    auto insert_job = [&tree](int start, int count) {
        for (int i = 0; i < count; ++i)
            tree.Insert(start + i, start + i);
    };

    std::vector<std::thread> threads;
    for (int t = 0; t < THREADS; ++t)
        threads.emplace_back(insert_job, t * EACH, EACH);

    for (auto& th : threads)
        th.join();

    // Si el mutex funciona, no se pierde ningún nodo
    assert(count_nodes(tree) == TOTAL &&
        "Mutex: se perdieron nodos por race condition (el mutex no funciona)");
    pass("Mutex: todos los nodos insertados concurrentemente estan presentes");
}

// ============================================================
// MAIN
// ============================================================
int main() {
    std::cout << "=======================================================\n";
    std::cout << "  BATERIA DE PRUEBAS UNITARIAS - CBinaryTree\n";
    std::cout << "=======================================================\n";

    TestCopyAndMove();
    TestInsertRemove();
    TestIterators();
    TestTraversals();
    TestVariadicTemplates();
    TestStreamOperators();
    TestConcurrency();

    std::cout << "\n=======================================================\n";
    std::cout << "  TODAS LAS PRUEBAS PASARON EXITOSAMENTE\n";
    std::cout << "=======================================================\n";
    return 0;
}