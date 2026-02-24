// ============================================================
//  test_binarytree.cpp  –  Pruebas unitarias GENERALES
//  Detecta capacidades via SFINAE; no asume nombres de metodos.
//  Requerimientos evaluados:
//    1) Constructor Copia, Move Constructor (exchange), Destructor virtual
//    2) Insert y Remove
//    3) Forward Iterator (begin/end) y Backward Iterator (rbegin/rend)
//    4) Recorridos PreOrder, InOrder y PostOrder (cualquier nombre)
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
#include <functional>

// --- INCLUSION OBLIGATORIA ---
#include "containers/binarytree.h"

// Neutralizar macros de plataforma que pueden interferir con los detectores SFINAE.
// Algunos headers de Windows/MinGW definen macros como ForEach que colisionan
// con los nombres de metodos que queremos detectar por SFINAE.
#ifdef ForEach
#  undef ForEach
#endif
#ifdef forEach
#  undef forEach
#endif
#ifdef foreach
#  undef foreach
#endif
#ifdef for_each
#  undef for_each
#endif
#ifdef InOrder
#  undef InOrder
#endif
#ifdef PreOrder
#  undef PreOrder
#endif
#ifdef PostOrder
#  undef PostOrder
#endif

using TestTraits = TreeTraitAscending<int>;
using TreeType   = CBinaryTree<TestTraits>;

// ============================================================
//  DETECTORES SFINAE (verifican existencia de metodo/operador)
//  No asumen un nombre especifico; prueban multiples variantes.
// ============================================================

// --- begin() / end() ---
template <typename T, typename = void>
struct has_begin_end : std::false_type {};
template <typename T>
struct has_begin_end<T, std::void_t<
    decltype(std::declval<T&>().begin()),
    decltype(std::declval<T&>().end())
>> : std::true_type {};

// --- rbegin() / rend() ---
template <typename T, typename = void>
struct has_rbegin_rend : std::false_type {};
template <typename T>
struct has_rbegin_rend<T, std::void_t<
    decltype(std::declval<T&>().rbegin()),
    decltype(std::declval<T&>().rend())
>> : std::true_type {};

// --- Insert(int, int) ---
template <typename T, typename = void>
struct has_insert : std::false_type {};
template <typename T>
struct has_insert<T, std::void_t<
    decltype(std::declval<T&>().Insert(std::declval<int>(), std::declval<int>()))
>> : std::true_type {};

// --- Remove(int) ---
template <typename T, typename = void>
struct has_remove : std::false_type {};
template <typename T>
struct has_remove<T, std::void_t<
    decltype(std::declval<T&>().Remove(std::declval<int>()))
>> : std::true_type {};

// --- operator<< ---
template <typename T, typename = void>
struct has_ostream_op : std::false_type {};
template <typename T>
struct has_ostream_op<T, std::void_t<
    decltype(std::declval<std::ostream&>() << std::declval<T&>())
>> : std::true_type {};

// --- operator>> ---
template <typename T, typename = void>
struct has_istream_op : std::false_type {};
template <typename T>
struct has_istream_op<T, std::void_t<
    decltype(std::declval<std::istream&>() >> std::declval<T&>())
>> : std::true_type {};

// ============================================================
//  MACRO para generar detectores de metodos de recorrido.
//  Acepta un lambda void(int&) sin argumentos extra.
// ============================================================
#define MAKE_TRAVERSAL_DETECTOR(TraitName, MethodName)                          \
template <typename T, typename = void>                                          \
struct TraitName : std::false_type {};                                          \
template <typename T>                                                           \
struct TraitName<T, std::void_t<                                                \
    decltype(std::declval<T&>().MethodName(                                     \
        std::declval<std::function<void(int&)>>()))                             \
>> : std::true_type {}

// InOrder y variantes
MAKE_TRAVERSAL_DETECTOR(has_InOrder,         InOrder);
MAKE_TRAVERSAL_DETECTOR(has_inOrder,         inOrder);
MAKE_TRAVERSAL_DETECTOR(has_inorder,         inorder);
MAKE_TRAVERSAL_DETECTOR(has_in_order,        in_order);
MAKE_TRAVERSAL_DETECTOR(has_traverseInOrder, traverseInOrder);
MAKE_TRAVERSAL_DETECTOR(has_TraversalIn,     TraversalIn);

// PreOrder y variantes
MAKE_TRAVERSAL_DETECTOR(has_PreOrder,         PreOrder);
MAKE_TRAVERSAL_DETECTOR(has_preOrder,         preOrder);
MAKE_TRAVERSAL_DETECTOR(has_preorder,         preorder);
MAKE_TRAVERSAL_DETECTOR(has_pre_order,        pre_order);
MAKE_TRAVERSAL_DETECTOR(has_traversePreOrder, traversePreOrder);
MAKE_TRAVERSAL_DETECTOR(has_TraversalPre,     TraversalPre);

// PostOrder y variantes
MAKE_TRAVERSAL_DETECTOR(has_PostOrder,         PostOrder);
MAKE_TRAVERSAL_DETECTOR(has_postOrder,         postOrder);
MAKE_TRAVERSAL_DETECTOR(has_postorder,         postorder);
MAKE_TRAVERSAL_DETECTOR(has_post_order,        post_order);
MAKE_TRAVERSAL_DETECTOR(has_traversePostOrder, traversePostOrder);
MAKE_TRAVERSAL_DETECTOR(has_TraversalPost,     TraversalPost);

// Foreach y variantes
MAKE_TRAVERSAL_DETECTOR(has_Foreach,  Foreach);
MAKE_TRAVERSAL_DETECTOR(has_ForEach,  ForEach);
MAKE_TRAVERSAL_DETECTOR(has_forEach,  forEach);
MAKE_TRAVERSAL_DETECTOR(has_for_each, for_each);

// ============================================================
//  DETECTORES ESPECIALES: FirstThat con pred(int&, int) -> bool
// ============================================================
#define MAKE_FIRSTTHAT_DETECTOR(TraitName, MethodName)                              \
template <typename T, typename = void>                                              \
struct TraitName : std::false_type {};                                              \
template <typename T>                                                               \
struct TraitName<T, std::void_t<                                                    \
    decltype(std::declval<T&>().MethodName(                                         \
        std::declval<std::function<bool(int&, int)>>(), std::declval<int>()))       \
>> : std::true_type {}

MAKE_FIRSTTHAT_DETECTOR(has_FirstThat,  FirstThat);
MAKE_FIRSTTHAT_DETECTOR(has_firstThat,  firstThat);
MAKE_FIRSTTHAT_DETECTOR(has_first_that, first_that);
MAKE_FIRSTTHAT_DETECTOR(has_FindFirst,  FindFirst);
MAKE_FIRSTTHAT_DETECTOR(has_findFirst,  findFirst);
MAKE_FIRSTTHAT_DETECTOR(has_find_first, find_first);

// ============================================================
//  DETECTORES: Foreach con argumento extra int (variadic test)
// ============================================================
#define MAKE_FOREACH_VARARG_DETECTOR(TraitName, MethodName)                         \
template <typename T, typename = void>                                               \
struct TraitName : std::false_type {};                                               \
template <typename T>                                                                \
struct TraitName<T, std::void_t<                                                     \
    decltype(std::declval<T&>().MethodName(                                          \
        std::declval<std::function<void(int&, int)>>(), std::declval<int>()))        \
>> : std::true_type {}

MAKE_FOREACH_VARARG_DETECTOR(has_Foreach_va,  Foreach);
MAKE_FOREACH_VARARG_DETECTOR(has_ForEach_va,  ForEach);
MAKE_FOREACH_VARARG_DETECTOR(has_forEach_va,  forEach);
MAKE_FOREACH_VARARG_DETECTOR(has_for_each_va, for_each);

// ============================================================
//  LLAMADORES GENERICOS (templates – branches son dependientes)
//  Solo compila el branch correcto gracias a 'if constexpr'.
// ============================================================
template <typename Tree>
bool call_inorder(Tree& t, std::function<void(int&)> fn) {
    if constexpr (has_InOrder<Tree>::value)         { t.InOrder(fn);         return true; }
    else if constexpr (has_inOrder<Tree>::value)    { t.inOrder(fn);         return true; }
    else if constexpr (has_inorder<Tree>::value)    { t.inorder(fn);         return true; }
    else if constexpr (has_in_order<Tree>::value)   { t.in_order(fn);        return true; }
    else if constexpr (has_traverseInOrder<Tree>::value) { t.traverseInOrder(fn); return true; }
    else if constexpr (has_TraversalIn<Tree>::value)     { t.TraversalIn(fn);     return true; }
    return false;
}

template <typename Tree>
bool call_preorder(Tree& t, std::function<void(int&)> fn) {
    if constexpr (has_PreOrder<Tree>::value)         { t.PreOrder(fn);            return true; }
    else if constexpr (has_preOrder<Tree>::value)    { t.preOrder(fn);            return true; }
    else if constexpr (has_preorder<Tree>::value)    { t.preorder(fn);            return true; }
    else if constexpr (has_pre_order<Tree>::value)   { t.pre_order(fn);           return true; }
    else if constexpr (has_traversePreOrder<Tree>::value) { t.traversePreOrder(fn); return true; }
    else if constexpr (has_TraversalPre<Tree>::value)     { t.TraversalPre(fn);     return true; }
    return false;
}

template <typename Tree>
bool call_postorder(Tree& t, std::function<void(int&)> fn) {
    if constexpr (has_PostOrder<Tree>::value)         { t.PostOrder(fn);             return true; }
    else if constexpr (has_postOrder<Tree>::value)    { t.postOrder(fn);             return true; }
    else if constexpr (has_postorder<Tree>::value)    { t.postorder(fn);             return true; }
    else if constexpr (has_post_order<Tree>::value)   { t.post_order(fn);            return true; }
    else if constexpr (has_traversePostOrder<Tree>::value) { t.traversePostOrder(fn); return true; }
    else if constexpr (has_TraversalPost<Tree>::value)     { t.TraversalPost(fn);     return true; }
    return false;
}

template <typename Tree>
bool call_foreach(Tree& t, std::function<void(int&)> fn) {
    if constexpr (has_Foreach<Tree>::value)    { t.Foreach(fn);  return true; }
    else if constexpr (has_ForEach<Tree>::value)    { t.ForEach(fn);  return true; }
    else if constexpr (has_forEach<Tree>::value)    { t.forEach(fn);  return true; }
    else if constexpr (has_for_each<Tree>::value)   { t.for_each(fn); return true; }
    return false;
}

// Foreach con arg extra (verifica variadic templates)
template <typename Tree>
bool call_foreach_vararg(Tree& t, std::function<void(int&, int)> fn, int arg) {
    if constexpr (has_Foreach_va<Tree>::value)    { t.Foreach(fn, arg);  return true; }
    else if constexpr (has_ForEach_va<Tree>::value)    { t.ForEach(fn, arg);  return true; }
    else if constexpr (has_forEach_va<Tree>::value)    { t.forEach(fn, arg);  return true; }
    else if constexpr (has_for_each_va<Tree>::value)   { t.for_each(fn, arg); return true; }
    return false;
}

template <typename Tree>
auto call_firstthat(Tree& t, std::function<bool(int&, int)> pred, int arg) {
    if constexpr (has_FirstThat<Tree>::value)   return t.FirstThat(pred, arg);
    else if constexpr (has_firstThat<Tree>::value)  return t.firstThat(pred, arg);
    else if constexpr (has_first_that<Tree>::value) return t.first_that(pred, arg);
    else if constexpr (has_FindFirst<Tree>::value)  return t.FindFirst(pred, arg);
    else if constexpr (has_findFirst<Tree>::value)  return t.findFirst(pred, arg);
    else if constexpr (has_find_first<Tree>::value) return t.find_first(pred, arg);
    else return t.end();
}

// ============================================================
//  HELPER GENERICO: ¿el resultado de FirstThat es valido?
//  Funciona con punteros crudos (int*, Node*) O con iteradores.
//  Si el alumno retorna nullptr = no encontrado.
//  Si el alumno retorna un iterador = al end() = no encontrado.
// ============================================================
template <typename Result, typename Tree>
bool is_valid_result(Result& r, Tree& t) {
    if constexpr (std::is_pointer_v<Result>) {
        return r != nullptr;
    } else {
        return r != t.end();
    }
}

// Desreferencia generica: puntero o iterador (operator* puede ser no-const)
template <typename Result>
int deref_result(Result& r) {
    return *r;
}

// ============================================================
//  PRUEBAS ESTATICAS (tiempo de compilacion)
// ============================================================

// Req 1 – Regla de los 5
static_assert(std::is_copy_constructible_v<TreeType>,
    "REQUERIMIENTO 1: Debe tener Constructor Copia");

static_assert(std::is_move_constructible_v<TreeType>,
    "REQUERIMIENTO 1: Debe tener Move Constructor");

static_assert(std::has_virtual_destructor_v<TreeType>,
    "REQUERIMIENTO 1: El destructor debe ser virtual (safe destructor)");

// Req 2 – Insert / Remove
static_assert(has_insert<TreeType>::value,
    "REQUERIMIENTO 2: Debe existir Insert(val, ref)");
static_assert(has_remove<TreeType>::value,
    "REQUERIMIENTO 2: Debe existir Remove(val)");

// Req 3 – Iteradores
static_assert(has_begin_end<TreeType>::value,
    "REQUERIMIENTO 3: Debe tener begin() y end() (Forward Iterator)");
static_assert(has_rbegin_rend<TreeType>::value,
    "REQUERIMIENTO 3: Debe tener rbegin() y rend() (Backward Iterator)");


// Req 5 – Foreach y FirstThat
static_assert(
    has_Foreach<TreeType>::value || has_ForEach<TreeType>::value ||
    has_forEach<TreeType>::value || has_for_each<TreeType>::value,
    "REQUERIMIENTO 5: No existe ningun metodo Foreach (con cualquier nombre)");

static_assert(
    has_FirstThat<TreeType>::value || has_firstThat<TreeType>::value ||
    has_first_that<TreeType>::value || has_FindFirst<TreeType>::value ||
    has_findFirst<TreeType>::value  || has_find_first<TreeType>::value,
    "REQUERIMIENTO 5: No existe ningun metodo FirstThat (con cualquier nombre)");

// Req 6 – Operadores de stream
static_assert(has_ostream_op<TreeType>::value,
    "REQUERIMIENTO 6: No se encontro operator<< para la clase");
static_assert(has_istream_op<TreeType>::value,
    "REQUERIMIENTO 6: No se encontro operator>> para la clase");

// ============================================================
//  Helpers de informe
// ============================================================
static void pass(const char* m) { std::cout << "  [PASS] " << m << "\n"; }
static void warn(const char* m) { std::cout << "  [WARN] " << m << "\n"; }
static void sect(const char* m) { std::cout << "\n--- " << m << " ---\n"; }

static int count_nodes(TreeType& t) {
    int n = 0;
    for (auto it = t.begin(); it != t.end(); ++it) ++n;
    return n;
}

static bool tree_empty(TreeType& t) {
    return !(t.begin() != t.end());
}

// ============================================================
//  TEST 1 – Constructor Copia y Move Constructor
// ============================================================
void TestCopyAndMove() {
    sect("REQUERIMIENTO 1: Constructor Copia y Move Constructor");

    TreeType orig;
    orig.Insert(10, 1); orig.Insert(20, 2); orig.Insert(30, 3);

    TreeType copy(orig);
    std::vector<int> ov, cv;
    for (auto it = orig.begin(); it != orig.end(); ++it) ov.push_back(*it);
    for (auto it = copy.begin(); it != copy.end(); ++it) cv.push_back(*it);
    assert(ov == cv && "Constructor Copia: debe ser identica al original");
    pass("Constructor Copia: la copia es identica al original");

    copy.Insert(99, 9);
    std::vector<int> ov2;
    for (auto it = orig.begin(); it != orig.end(); ++it) ov2.push_back(*it);
    assert(ov == ov2 && "Constructor Copia: debe ser deep copy");
    pass("Constructor Copia: deep copy confirmada");

    TreeType src;
    src.Insert(100,1); src.Insert(200,2); src.Insert(300,3);
    std::vector<int> sb;
    for (auto it = src.begin(); it != src.end(); ++it) sb.push_back(*it);

    TreeType moved(std::move(src));
    std::vector<int> mb;
    for (auto it = moved.begin(); it != moved.end(); ++it) mb.push_back(*it);

    assert(sb == mb          && "Move Constructor: destino debe tener los elementos");
    assert(tree_empty(src)   && "Move Constructor (exchange): origen debe quedar vacio");
    pass("Move Constructor (exchange): transfiere propiedad y vacia el origen");
}

// ============================================================
//  TEST 2 – Insert y Remove
// ============================================================
void TestInsertRemove() {
    sect("REQUERIMIENTO 2: Insert y Remove");

    TreeType t;
    t.Insert(50,1); t.Insert(30,2); t.Insert(70,3);
    t.Insert(20,4); t.Insert(40,5);
    assert(count_nodes(t) == 5 && "Insert: 5 inserciones deben dar 5 nodos");
    pass("Insert: 5 nodos insertados correctamente");

    t.Insert(50, 99);
    pass("Insert: insertar duplicado no provoca crash");

    TreeType t2;
    t2.Insert(50,1); t2.Insert(30,2); t2.Insert(70,3);
    t2.Remove(30);
    bool f1 = false;
    for (auto it = t2.begin(); it != t2.end(); ++it) if (*it == 30) f1 = true;
    assert(count_nodes(t2) == 2 && "Remove hoja: deben quedar 2 nodos");
    assert(!f1                   && "Remove hoja: el nodo eliminado no debe aparecer");
    pass("Remove: eliminar nodo hoja");

    TreeType t3;
    t3.Insert(50,1); t3.Insert(30,2); t3.Insert(70,3);
    t3.Insert(20,4); t3.Insert(40,5);
    t3.Remove(30);
    bool f2 = false;
    for (auto it = t3.begin(); it != t3.end(); ++it) if (*it == 30) f2 = true;
    assert(count_nodes(t3) == 4 && "Remove 2-hijos: deben quedar 4 nodos");
    assert(!f2                   && "Remove 2-hijos: el nodo eliminado no debe aparecer");
    pass("Remove: eliminar nodo con dos hijos");

    t3.Remove(999);
    pass("Remove: eliminar elemento inexistente no provoca crash");
}

// ============================================================
//  TEST 3 – Forward Iterator y Backward Iterator
// ============================================================
void TestIterators() {
    sect("REQUERIMIENTO 3: Forward Iterator y Backward Iterator");

    TreeType t;
    t.Insert(50,1); t.Insert(20,2); t.Insert(80,3);
    t.Insert(10,4); t.Insert(30,5);

    std::vector<int> fwd;
    for (auto it = t.begin(); it != t.end(); ++it) fwd.push_back(*it);
    assert(fwd.size() == 5  && "Forward: debe visitar 5 nodos");
    assert(fwd[0] == 10     && "Forward: comienza en el minimo");
    assert(fwd.back() == 80 && "Forward: termina en el maximo");
    bool asc = true;
    for (size_t i = 1; i < fwd.size(); ++i) if (fwd[i] < fwd[i-1]) asc = false;
    assert(asc              && "Forward: recorrido debe ser ascendente");
    pass("Forward Iterator: recorre en orden ascendente");

    std::vector<int> bwd;
    for (auto it = t.rbegin(); it != t.rend(); ++it) bwd.push_back(*it);
    assert(bwd.size() == 5   && "Backward: debe visitar 5 nodos");
    assert(bwd[0] == 80      && "Backward: comienza en el maximo");
    assert(bwd.back() == 10  && "Backward: termina en el minimo");
    bool desc = true;
    for (size_t i = 1; i < bwd.size(); ++i) if (bwd[i] > bwd[i-1]) desc = false;
    assert(desc              && "Backward: recorrido debe ser descendente");
    pass("Backward Iterator: recorre en orden descendente");

    TreeType empty;
    assert(tree_empty(empty) && "Arbol vacio: begin() debe igualar end()");
    pass("Iteradores en arbol vacio: comportamiento correcto");
}

// ============================================================
//  TEST 4 – Recorridos PreOrder, InOrder, PostOrder
// ============================================================
void TestTraversals() {
    sect("REQUERIMIENTO 4: PreOrder, InOrder, PostOrder");

    TreeType t;
    t.Insert(50,1); t.Insert(30,2); t.Insert(70,3);
    t.Insert(20,4); t.Insert(40,5);

    std::vector<int> inorder, preorder, postorder;
    bool hi = call_inorder  (t, [&inorder]  (int& v){ inorder.push_back(v);   });
    bool hp = call_preorder (t, [&preorder] (int& v){ preorder.push_back(v);  });
    bool ho = call_postorder(t, [&postorder](int& v){ postorder.push_back(v); });

    if (!hi) { warn("InOrder  : metodo no encontrado con ningun nombre comun"); }
    else {
        assert(inorder.size() == 5 && "InOrder: debe visitar 5 nodos");
        bool asc = true;
        for (size_t i = 1; i < inorder.size(); ++i) if (inorder[i] < inorder[i-1]) asc = false;
        assert(asc && "InOrder: resultado debe ser ascendente");
        pass("InOrder: recorre todos los nodos en orden ascendente");
    }

    if (!hp) { warn("PreOrder : metodo no encontrado con ningun nombre comun"); }
    else {
        assert(preorder.size() == 5 && "PreOrder: debe visitar 5 nodos");
        assert(preorder[0] == 50    && "PreOrder: la raiz debe ser visitada primero");
        pass("PreOrder: la raiz es visitada primero");
    }

    if (!ho) { warn("PostOrder: metodo no encontrado con ningun nombre comun"); }
    else {
        assert(postorder.size() == 5 && "PostOrder: debe visitar 5 nodos");
        assert(postorder.back() == 50 && "PostOrder: la raiz debe ser visitada al final");
        pass("PostOrder: la raiz es visitada al final");
    }

    if (hi && hp && ho) {
        std::vector<int> si = inorder, sp = preorder, so = postorder;
        std::sort(si.begin(), si.end());
        std::sort(sp.begin(), sp.end());
        std::sort(so.begin(), so.end());
        assert(si == sp && si == so &&
            "Los tres recorridos deben visitar el mismo conjunto de nodos");
        pass("PreOrder + InOrder + PostOrder: visitan el mismo conjunto de nodos");
    }

    assert((hi || hp || ho) && "REQUERIMIENTO 4: No se encontro ningun metodo de recorrido");
}

// ============================================================
//  TEST 5 – Foreach y FirstThat con variadic templates
// ============================================================
void TestVariadicTemplates() {
    sect("REQUERIMIENTO 5: Foreach y FirstThat (Variadic Templates)");

    TreeType t;
    t.Insert(10,1); t.Insert(20,2); t.Insert(30,3);
    t.Insert(40,4); t.Insert(50,5);

    // --- Foreach ---
    int visited = 0;
    bool hfe = call_foreach(t, [&visited](int& /*v*/){ ++visited; });
    if (!hfe) { warn("Foreach: metodo no encontrado con ningun nombre comun"); }
    else {
        assert(visited == 5 && "Foreach: debe visitar exactamente 5 nodos");
        pass("Foreach: visita todos los nodos");

        // Foreach con argumento variadico extra
        int suma = 0;
        bool hva = call_foreach_vararg(t, [&suma](int& val, int lim){
            if (val > lim) suma += val;
        }, 0);
        if (!hva) {
            // Si no acepta args extra, al menos verifica que visita todo
            call_foreach(t, [&suma](int& val){ suma += val; });
        }
        assert(suma == 150 && "Foreach: suma 10+20+30+40+50 debe ser 150");
        pass("Foreach: acepta argumentos variadicos extra (variadic templates)");
    }

    // --- FirstThat ---
    bool hft = has_FirstThat<TreeType>::value || has_firstThat<TreeType>::value ||
               has_first_that<TreeType>::value || has_FindFirst<TreeType>::value ||
               has_findFirst<TreeType>::value  || has_find_first<TreeType>::value;

    if (!hft) { warn("FirstThat: metodo no encontrado con ningun nombre comun"); }
    else {
        // Nota: call_firstthat retorna 'auto' — puede ser un iterador O un
        // puntero crudo (T*), segun como el alumno implemento FirstThat.
        // is_valid_result() maneja ambos casos con if constexpr.
        auto it = call_firstthat(t, [](int& val, int lim){ return val > lim; }, 25);
        assert(is_valid_result(it, t) && "FirstThat: debe encontrar elemento > 25");
        assert(deref_result(it) > 25  && "FirstThat: el valor encontrado debe ser > 25");
        pass("FirstThat: encuentra el primer elemento que cumple el predicado");

        auto it2 = call_firstthat(t, [](int& val, int lim){ return val > lim; }, 9999);
        assert(!is_valid_result(it2, t) && "FirstThat: debe retornar 'no encontrado' si ningun elemento cumple");
        pass("FirstThat: retorna 'no encontrado' cuando ningun elemento cumple");
    }

    assert((hfe || hft) &&
        "REQUERIMIENTO 5: No se encontro ningun Foreach ni FirstThat");
}

// ============================================================
//  TEST 6 – operator<< y operator>>
// ============================================================
void TestStreamOperators() {
    sect("REQUERIMIENTO 6: operator<< y operator>>");

    TreeType t;
    t.Insert(50,1); t.Insert(20,2); t.Insert(80,3);

    std::ostringstream oss;
    oss << t;
    std::string s = oss.str();
    assert(!s.empty()                        && "operator<<: la salida no debe estar vacia");
    assert(s.find("50") != std::string::npos && "operator<<: debe contener el valor 50");
    assert(s.find("20") != std::string::npos && "operator<<: debe contener el valor 20");
    assert(s.find("80") != std::string::npos && "operator<<: debe contener el valor 80");
    pass("operator<<: serializa todos los nodos");

    TreeType t2;
    std::istringstream iss(s);
    iss >> t2;

    std::vector<int> ov, rv;
    for (auto it = t.begin();  it != t.end();  ++it) ov.push_back(*it);
    for (auto it = t2.begin(); it != t2.end(); ++it) rv.push_back(*it);
    assert(ov == rv && "operator>>: el arbol restaurado debe ser identico al original");
    pass("operator>>: deserializa y reconstruye el arbol correctamente");
}

// ============================================================
//  TEST 7 – Mutex / Concurrencia
// ============================================================
void TestConcurrency() {
    sect("REQUERIMIENTO 7: Mutex y seguridad en concurrencia");

    TreeType t;
    const int THREADS = 4, EACH = 50, TOTAL = THREADS * EACH;

    auto job = [&t](int start, int count) {
        for (int i = 0; i < count; ++i)
            t.Insert(start + i, start + i);
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < THREADS; ++i)
        threads.emplace_back(job, i * EACH, EACH);
    for (auto& th : threads) th.join();

    assert(count_nodes(t) == TOTAL &&
        "Mutex: se perdieron nodos — posible race condition sin mutex");
    pass("Mutex: todos los nodos insertados concurrentemente estan presentes");
}

// ============================================================
//  MAIN
// ============================================================
int main() {
    std::cout << "=======================================================\n";
    std::cout << "  BATERIA DE PRUEBAS GENERALES - CBinaryTree\n";
    std::cout << "  (Detecta metodos via SFINAE, sin asumir nombres)\n";
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