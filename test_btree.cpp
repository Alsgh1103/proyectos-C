#include <iostream>
#include <type_traits>
#include <cassert>
#include <thread>
#include <vector>
#include <sstream>

// Incluye los archivos necesarios de tu proyecto
#include "BTree.h" 

// --- 1. CONFIGURACIÓN DEL ENTORNO DE PRUEBA ---
// Necesitamos un Traits de prueba para instanciar el BTree
struct TestTraits {
    using value_type = int;
    using ref_type = int;
};

using TreeType = BTree<TestTraits>;

// --- 2. PRUEBAS ESTÁTICAS (Static Assertions) ---
// Si alguna de estas pruebas falla, el código NO compilará.

// A. Verificación de Constructores y Destructor Safe (Regla de los 5)
static_assert(std::is_copy_constructible_v<TreeType>, 
              "ERROR: El BTree debe tener un Constructor Copia");
static_assert(std::is_move_constructible_v<TreeType>, 
              "ERROR: El BTree debe tener un Move Constructor");
static_assert(std::has_virtual_destructor_v<TreeType>, 
              "ERROR: El Destructor del BTree debe ser 'virtual' (Safe Destructor)");

// B. Detección SFINAE para Sobrecarga de Operadores (<< y >>)
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
    std::cout << "[Test] Verificando Forward y Backward Iterators..." << std::endl;
    TreeType tree(3, true);
    tree.Insert(10, 100);
    tree.Insert(20, 200);

    // Forward Iterator test (Verifica si el tipo itera hacia adelante)
    auto it = tree.begin();
    assert(it != tree.end() && "El iterador forward no debe estar vacio");
    ++it; // Operador de incremento

    // Backward Iterator test
    auto rit = tree.rbegin();
    assert(rit != tree.rend() && "El iterador backward no debe estar vacio");
    ++rit; // Operador de decremento interno

    std::cout << "  ✓ Iteradores pasaron la prueba." << std::endl;
}

void TestVariadicTemplates() {
    std::cout << "[Test] Verificando Foreach y FirstThat con Variadic Templates..." << std::endl;
    TreeType tree(3, true);
    tree.Insert(15, 150);
    tree.Insert(25, 250);

    int suma_test = 0;
    std::string mensaje_test = "";

    // A. Prueba de Foreach con múltiples parámetros (Variadic Templates)
    // Pasamos un lambda, un acumulador por referencia y un string.
    tree.Foreach([&](typename decltype(tree)::ObjectInfo& info, int level, int& acumulador, std::string prefijo) {
        acumulador += info.key; // Asumiendo que ObjectInfo tiene 'key'
        mensaje_test = prefijo;
    }, suma_test, std::string("Procesado"));

    // B. Prueba de FirstThat con parámetros adicionales
    auto* found = tree.FirstThat([](typename decltype(tree)::ObjectInfo& info, int level, int limite) {
        return info.key > limite;
    }, 20); // 20 es el argumento extra (limite) que recibe el Variadic Template

    assert(found != nullptr && "FirstThat no funcionó correctamente");
    std::cout << "  ✓ Variadic Templates pasaron la prueba." << std::endl;
}

void TestConcurrencyMutex() {
    std::cout << "[Test] Verificando control de concurrencia (Mutex)..." << std::endl;
    TreeType tree(5, true);

    // Creamos múltiples hilos que insertan simultáneamente
    auto insert_job = [&tree](int start, int count) {
        for (int i = start; i < start + count; ++i) {
            tree.Insert(i, i * 10);
        }
    };

    std::thread t1(insert_job, 1, 100);
    std::thread t2(insert_job, 101, 100);
    std::thread t3(insert_job, 201, 100);

    t1.join();
    t2.join();
    t3.join();

    // Si hubo Race Conditions y el mutex falló, el tamaño del árbol sería impredecible o habría Segmentation Fault.
    assert(tree.size() == 300 && "Fallo en la concurrencia: El tamaño del árbol es incorrecto");
    std::cout << "  ✓ Control de Concurrencia (Mutex) pasó la prueba de estrés." << std::endl;
}

void TestOperators() {
    std::cout << "[Test] Verificando Operadores IO..." << std::endl;
    TreeType tree;
    tree.Insert(1, 10);
    
    // Prueba Operador <<
    std::ostringstream oss;
    oss << tree;
    assert(oss.str().length() > 0 && "Operador << no inyectó datos al flujo");

    std::cout << "  ✓ Operadores pasaron la prueba." << std::endl;
}

int main() {
    std::cout << "=== INICIANDO BATERÍA DE PRUEBAS DEL BTREE ===" << std::endl;
    
    TestIterators();
    TestVariadicTemplates();
    TestConcurrencyMutex();
    TestOperators();

    std::cout << "=== TODAS LAS PRUEBAS FUERON EXITOSAS ===" << std::endl;
    return 0; // Código 0 = GitHub Actions da el CHECK VERDE ✅
}
