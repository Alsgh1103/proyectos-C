#ifndef __AVL_H__
#define __AVL_H__

#include <iostream>
#include <utility>
#include <mutex>
#include "../general/types.h"
#include "../util.h"
#include "treetraits.h"

//Forward declarations
template<typename Traits> class NodeAVL;
template<typename Traits> class CAVL;
template<typename Traits> class CAVLForwardIterator;
template<typename Traits> class CAVLBackwardIterator;

//Backward Iterator
template<typename Traits>
class CAVLBackwardIterator{
    using BackwardIterator = CAVLBackwardIterator<Traits>;
    using Node             = NodeAVL<Traits>;
    using value_type       = typename Traits::value_type;
private:
    Node *m_pCurrent = nullptr;
public:
    CAVLBackwardIterator(Node *pCurrent)
        : m_pCurrent(pCurrent)
    {}
    value_type& operator*();
    bool operator!=(const BackwardIterator& another) const;
    bool operator==(const BackwardIterator& another) const;
    BackwardIterator& operator++();
};

//Funciones BackwardIterator
template <typename Traits>
typename CAVLBackwardIterator<Traits>::value_type& 
CAVLBackwardIterator<Traits>::operator*(){
    return m_pCurrent->GetValueRef();
}

template <typename Traits>
bool CAVLBackwardIterator<Traits>::operator!=(const CAVLBackwardIterator<Traits>& another) const{
    return  m_pCurrent != another.m_pCurrent;
}
template <typename Traits>
bool CAVLBackwardIterator<Traits>::operator==(const CAVLBackwardIterator<Traits>& another) const{
    return m_pCurrent == another.m_pCurrent;
}
template <typename Traits>
CAVLBackwardIterator<Traits>& CAVLBackwardIterator<Traits>::operator++(){
    if (m_pCurrent->m_pChild[0]) {
        m_pCurrent = m_pCurrent->m_pChild[0];
        while (m_pCurrent->m_pChild[1]) {
            m_pCurrent = m_pCurrent->m_pChild[1];
        }
    } else {
        Node *pParent = m_pCurrent->m_pParent;
        while (pParent && m_pCurrent == pParent->m_pChild[0]) {
            m_pCurrent = pParent;
            pParent = pParent->m_pParent;
        }
        m_pCurrent = pParent;
    }
    return *this;
}

//Forward Iterador 
template <typename Traits>
class CAVLForwardIterator{
    using ForwardIterator = CAVLForwardIterator<Traits>;
    using Node            = NodeAVL<Traits>;
    using value_type      = typename Traits::value_type;
private:
    Node *m_pCurrent = nullptr;

public:
    CAVLForwardIterator(Node *pCurrent)
        : m_pCurrent(pCurrent)
    {}
    value_type& operator*();
    bool operator!=(const ForwardIterator& another) const;
    bool operator==(const ForwardIterator& another) const;
    ForwardIterator& operator++();

};

//Funciones ForwardIterator
template <typename Traits>
typename CAVLForwardIterator<Traits>::value_type& CAVLForwardIterator<Traits>::operator*(){ return m_pCurrent->GetValueRef(); }
template <typename Traits>
bool CAVLForwardIterator<Traits>::operator!=(const CAVLForwardIterator<Traits>& another) const { return m_pCurrent!=another.m_pCurrent; }
template <typename Traits>
bool CAVLForwardIterator<Traits>::operator==(const CAVLForwardIterator<Traits>& another) const { return m_pCurrent==another.m_pCurrent; }

template <typename Traits>
CAVLForwardIterator<Traits>& CAVLForwardIterator<Traits>::operator++() {
    if (m_pCurrent->m_pChild[1]) {
        m_pCurrent = m_pCurrent->m_pChild[1];
        while (m_pCurrent->m_pChild[0]) {
            m_pCurrent = m_pCurrent->m_pChild[0];
        }
    } else {
        Node *pParent = m_pCurrent->m_pParent;
        while (pParent && m_pCurrent == pParent->m_pChild[1]) {
            m_pCurrent = pParent;
            pParent = pParent->m_pParent;
        }
        m_pCurrent = pParent;
    }
    return *this;
}



template <typename Traits>
class NodeAVL{
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeAVL<Traits>;
    using  CompareFunc = typename Traits::CompareFunc;
private:
    value_type  m_data;
    ref_type    m_ref;
    size_t      m_height     = 1;
    Node       *m_pParent    =  nullptr ; 
    Node       *m_pChild[2]  = {nullptr};
public:
    NodeAVL(){}
    NodeAVL( value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref){   }
    value_type  GetValue   () const { return m_data; }
    value_type &GetValueRef()       { return m_data; }
    ref_type    GetRef     () const { return m_ref;  }
    ref_type   &GetRefRef  ()       { return m_ref;  }
    size_t      GetHeight  () const { return m_height; }

    friend class CAVL<Traits>;
    friend class CAVLForwardIterator<Traits>;
    friend class CAVLBackwardIterator<Traits>;
};

template <typename Traits>
class CAVL{
public:
    using  value_type         = typename Traits::value_type;
    using  Node               = NodeAVL<Traits>;
    using  ForwardIterator    = CAVLForwardIterator<Traits>;
    using  CompareFunc        = typename Traits::CompareFunc;
    using  BackwardIterator   = CAVLBackwardIterator<Traits>;
private:
    Node *m_pRoot = nullptr;
    CompareFunc comp;
    mutable std::recursive_mutex m_mtx;

public:
    CAVL(){}
    // TODO: Copy constructor
    CAVL(const CAVL &another);
    // TODO: Move constructor
    CAVL(CAVL &&another) noexcept {
        std::lock_guard<std::recursive_mutex> lock(another.m_mtx);
        m_pRoot = std::exchange(another.m_pRoot, nullptr);
    }
    virtual ~CAVL(){
        Destroy(m_pRoot);
    }
private:
    void InternalInsert(Node *&rpCurrent, Node *pParent, const value_type &val, ref_type ref){
        if( !rpCurrent ){
            rpCurrent = new Node(val, ref);
            rpCurrent->m_pParent = pParent;
            return;
        }
        auto path = comp(val, rpCurrent->GetValue());
        InternalInsert(rpCurrent->m_pChild[path], rpCurrent, val, ref);
        Balance(rpCurrent);
    }

    Node* Clone(Node *pCurrent){
        if (!pCurrent)
            return nullptr;
        Node *pNewNode = new Node(pCurrent->GetValue(),pCurrent->GetRef());
        pNewNode->m_height = pCurrent->m_height;
        if (pCurrent->m_pChild[0]){
            pNewNode->m_pChild[0]            = Clone(pCurrent->m_pChild[0]);
            pNewNode->m_pChild[0]->m_pParent = pNewNode;
        }
        if (pCurrent->m_pChild[1]){
            pNewNode->m_pChild[1]            = Clone(pCurrent->m_pChild[1]);
            pNewNode->m_pChild[1]->m_pParent = pNewNode;
        }
        return pNewNode;
    }
    void Destroy(Node *pCurrent){
        if (!pCurrent)
            return;
        if (pCurrent->m_pChild[0])
            Destroy(pCurrent->m_pChild[0]);
        if (pCurrent->m_pChild[1]) 
            Destroy(pCurrent->m_pChild[1]);
        delete pCurrent;
    }

    //ENCONTRAR EL INMEDIATO SUPERIOR (sucesor in-orden)
    Node* FindMin(Node* pNode){
        while (pNode->m_pChild[0])
            pNode = pNode->m_pChild[0];
        return pNode;
    }
    //INORDEN VARIADIC
    template <typename Func, typename... Args>
    void InternalInorden(Node *pCurrent, Func fn, Args&&... args) {
        if (pCurrent) {
            InternalInorden(pCurrent->m_pChild[0], fn, std::forward<Args>(args)...);
            fn(pCurrent->GetValueRef(), args...);
            InternalInorden(pCurrent->m_pChild[1], fn, std::forward<Args>(args)...);
        }
    }

    //PREORDEN VARIADIC
    template <typename Func, typename... Args>
    void InternalPreorden(Node *pCurrent, Func fn, Args&&... args) {
        if (pCurrent) {
            fn(pCurrent->GetValueRef(), args...);
            InternalPreorden(pCurrent->m_pChild[0], fn, std::forward<Args>(args)...);
            InternalPreorden(pCurrent->m_pChild[1], fn, std::forward<Args>(args)...);
        }
    }

    //POSTODEN VARIADIC
    template <typename Func, typename... Args>
    void InternalPostorden(Node *pCurrent, Func fn, Args&&... args) {
        if (pCurrent) {
            InternalPostorden(pCurrent->m_pChild[0], fn, std::forward<Args>(args)...);
            InternalPostorden(pCurrent->m_pChild[1], fn, std::forward<Args>(args)...);
            fn(pCurrent->GetValueRef(), args...);
        }
    }

    //FOREACH VARIADIC
    template <typename Func, typename... Args>
    void InternalForeach(Node *pCurrent, Func fn, Args&&... args){
        if (pCurrent){
            InternalForeach(pCurrent->m_pChild[0], fn, std::forward<Args>(args)...);
            fn(pCurrent->GetValueRef(), args...);
            InternalForeach(pCurrent->m_pChild[1], fn, std::forward<Args>(args)...);
        }
    }

    //FIRSTHAT VARIADIC
    template <typename Func, typename... Args>
    Node* InternalFirstThat(Node* pCurrent, Func fn, Args&&... args){
        if (!pCurrent)
            return nullptr;
        Node *pFound = InternalFirstThat(pCurrent->m_pChild[0], fn, std::forward<Args>(args)...);
        if (pFound)
            return pFound;
        if ( fn(pCurrent->GetValueRef(), args...) )
            return pCurrent;
        return InternalFirstThat(pCurrent->m_pChild[1], fn, std::forward<Args>(args)...);
    }

    //REMOVE
    void InternalRemove(Node *&rpCurrent, const value_type& value){
        if (!rpCurrent)
            return;
        if ( value == rpCurrent->GetValueRef() ){
            //caso 1 - nodo hoja
            if (rpCurrent->m_pChild[0] == nullptr && rpCurrent->m_pChild[1] == nullptr){
                delete rpCurrent;
                rpCurrent = nullptr;
                return;
            }
            //caso 2a - nodo tiene solo hijo derecho
            if (!rpCurrent->m_pChild[0] && rpCurrent->m_pChild[1]){
                Node* pChild      = rpCurrent->m_pChild[1];
                pChild->m_pParent = rpCurrent->m_pParent;
                delete rpCurrent;
                rpCurrent = pChild;
                return;
            }
            //caso 2b - nodo tiene solo hijo izquierdo
            if (rpCurrent->m_pChild[0] && !rpCurrent->m_pChild[1]){
                Node* pChild      = rpCurrent->m_pChild[0];
                pChild->m_pParent = rpCurrent->m_pParent;
                delete rpCurrent;
                rpCurrent = pChild;
                return;
            }
            //caso 3 - dos hijos
            Node* pSucc = FindMin(rpCurrent->m_pChild[1]);
            rpCurrent->GetValueRef() = pSucc->GetValue();
            rpCurrent->GetRefRef()   = pSucc->GetRef();
            InternalRemove(rpCurrent->m_pChild[1], pSucc->GetValue());
            Balance(rpCurrent);
            return;

        }
        auto path = comp (value, rpCurrent->GetValueRef());
        InternalRemove(rpCurrent->m_pChild[path], value);
        Balance(rpCurrent);
    }

    //IMPRIMIR ARBOL
    void InternalPrintTree(Node* pNode, int depth) {
        if (!pNode) return;
        InternalPrintTree(pNode->m_pChild[0], depth + 1);
        for (int i = 0; i < depth; i++) std::cout << "\t";
        std::cout << "(" << pNode->GetValue() << "," << pNode->GetHeight() << ")\n";
        InternalPrintTree(pNode->m_pChild[1], depth + 1);
    }
    
    //Funcion Balancear
    void Balance(Node *&pCurrent){
        UpdateHeight(pCurrent);
        if (GetBalance(pCurrent) >= 2){
            if(GetBalance(pCurrent->m_pChild[0]) >= 0){
                RotateRight(pCurrent);
            }
            else{
                RotateLeft(pCurrent->m_pChild[0]);
                RotateRight(pCurrent);
            }
            return;
        }
        if (GetBalance(pCurrent) <= -2){
            if (GetBalance(pCurrent->m_pChild[1]) <= 0){
                RotateLeft(pCurrent);
            }else{
                RotateRight(pCurrent->m_pChild[1]);
                RotateLeft(pCurrent);
            }
            return;
        }
    }

    //Funcion rotar a la derecha
    void RotateRight (Node *&rpCurrent){
        Node* pLeftSon       = rpCurrent->m_pChild[0]; 
        rpCurrent->m_pChild[0]     = pLeftSon->m_pChild[1];
        
        if (pLeftSon->m_pChild[1])
            rpCurrent->m_pChild[0]->m_pParent = rpCurrent;

        pLeftSon->m_pParent   = rpCurrent->m_pParent;
        pLeftSon->m_pChild[1] = rpCurrent;
        rpCurrent->m_pParent        = pLeftSon;
        UpdateHeight(rpCurrent);
        UpdateHeight(pLeftSon);
        rpCurrent = pLeftSon;
    }

    //Funcion rotar a la izquierda
    void RotateLeft (Node *&rpCurrent){
            Node* pRightSon       = rpCurrent->m_pChild[1]; 
        rpCurrent->m_pChild[1]     = pRightSon->m_pChild[0];
        
        if (pRightSon->m_pChild[0])
            rpCurrent->m_pChild[1]->m_pParent = rpCurrent;

        pRightSon->m_pParent   = rpCurrent->m_pParent;
        pRightSon->m_pChild[0] = rpCurrent;
        rpCurrent->m_pParent        = pRightSon;
        UpdateHeight(rpCurrent);
        UpdateHeight(pRightSon);
        rpCurrent = pRightSon;
    }

    //Funciones auxiliares del AVL 
    size_t GetHeight(Node* pCurrent){
        if (!pCurrent)
            return 0;
        return pCurrent->GetHeight();
    }

    void UpdateHeight(Node* pCurrent){
        pCurrent->m_height = 1 + std::max(GetHeight(pCurrent->m_pChild[0]),GetHeight(pCurrent->m_pChild[1]));
    }
    T1 GetBalance(Node* pCurrent){
        return (T1)GetHeight(pCurrent->m_pChild[0]) - (T1)GetHeight(pCurrent->m_pChild[1]);
    }

    //Operator <<
    friend ostream& operator<<(ostream& os, CAVL<Traits>& AVL){
        std::lock_guard<std::recursive_mutex> lock(AVL.m_mtx);
        os << "CAVL" << std::endl;
        os << "[";
            bool first = true;
            for (auto it = AVL.begin(); it != AVL.end(); ++it){
                if (!first)
                    os << " -> ";
                os << *it;
                first = false;
            }
        os << "]";
        return os;
    }

    //Operator >>
    friend istream& operator>>(istream& is, CAVL<Traits>& AVL){
        std::lock_guard<std::recursive_mutex> lock(AVL.m_mtx);
        size_t nElements;
        is >> nElements;
        for (size_t i = 0; i < nElements; ++i){
            value_type val;
            ref_type   ref;
            is >> val >> ref;
            AVL.Insert(val, ref);
        }
        return is;
    }

public:
    void Insert(const value_type &val, ref_type ref){
        std::lock_guard<std::recursive_mutex> lock(m_mtx);
        InternalInsert(m_pRoot, nullptr, val, ref);
    }

    template <typename Func, typename... Args>
    void Preorden(Func fn, Args ...args) {
        std::lock_guard<std::recursive_mutex> lock(m_mtx);
        InternalPreorden(m_pRoot, fn, args...);
    }

    template <typename Func, typename... Args>
    void Postorden(Func fn, Args ...args) {
        std::lock_guard<std::recursive_mutex> lock(m_mtx);
        InternalPostorden(m_pRoot, fn , args...);
    }

    template <typename Func, typename... Args>
    void Inorden(Func fn, Args... args) {
        std::lock_guard<std::recursive_mutex> lock(m_mtx);
        InternalInorden(m_pRoot, fn, args...);
    }

    //FOREACH VARIADIC PUBLICO
    template <typename Func, typename... Args>
    void Foreach(Func fn, Args... args){
        std::lock_guard<std::recursive_mutex> lock(m_mtx);
        InternalForeach(m_pRoot, fn, args...);
    }

    //FIRSTTHAT VARIADIC - PUBLICO
    template <typename Func, typename... Args>
    value_type* FirstThat(Func fn, Args... args){
        std::lock_guard<std::recursive_mutex> lock(m_mtx);
        Node *pFound = InternalFirstThat(m_pRoot, fn, args...);
        if (pFound)
            return &pFound->GetValueRef();
        return nullptr;
    }

    //Remove
    void Remove(const value_type& value){
        std::lock_guard<std::recursive_mutex> lock(m_mtx);
        InternalRemove(m_pRoot, value);
    }

    void PrintTree(){
        std::lock_guard<std::recursive_mutex> lock(m_mtx);
        if (!m_pRoot) { std::cout << "(arbol vacio)\n"; return; }
        InternalPrintTree(m_pRoot, 0);
    }

    CAVL& operator=(const CAVL& another);
    CAVL& operator=(CAVL&& another) noexcept;
    ForwardIterator  begin();
    ForwardIterator  end();
    BackwardIterator rbegin();
    BackwardIterator rend();
};


//ITERADORES - ForwardIterator
template <typename Traits>
typename CAVL<Traits>::ForwardIterator CAVL<Traits>::begin(){
    if (!m_pRoot)
        return ForwardIterator(nullptr);
    Node *pCurrent = m_pRoot;
    while (pCurrent->m_pChild[0])
        pCurrent = pCurrent->m_pChild[0];
    return ForwardIterator(pCurrent); 
}

template <typename Traits>
typename CAVL<Traits>::ForwardIterator 
CAVL<Traits>::end(){ return ForwardIterator(nullptr); }

//ITERADORES - BackwardIterator
template <typename Traits>
typename CAVL<Traits>::BackwardIterator 
CAVL<Traits>::rbegin() {
    if (!m_pRoot)
        return BackwardIterator(nullptr);
    Node *pCurrent = m_pRoot;
    while (pCurrent->m_pChild[1])
        pCurrent = pCurrent->m_pChild[1];
    return BackwardIterator(pCurrent);
}
template <typename Traits>
typename CAVL<Traits>::BackwardIterator 
CAVL<Traits>::rend(){ return BackwardIterator(nullptr); }

//Copy Constructor
template <typename Traits>
CAVL<Traits>::CAVL(const CAVL<Traits> &another){
    std::lock_guard<std::recursive_mutex> lock(another.m_mtx);
    m_pRoot = Clone(another.m_pRoot);
}

//Operador de asignación '=' para copy constructor
template <typename Traits>
CAVL<Traits>& CAVL<Traits>::operator=(const CAVL& another){
    if (this == &another)
        return *this;
    std::lock(m_mtx, another.m_mtx);
    std::lock_guard<std::recursive_mutex> lock1(m_mtx, std::adopt_lock);
    std::lock_guard<std::recursive_mutex> lock2(another.m_mtx, std::adopt_lock);
    Destroy(m_pRoot);
    m_pRoot = Clone(another.m_pRoot);
    return *this;
}
//Operador de asignación '=' para move constructor
template <typename Traits>
CAVL<Traits>& CAVL<Traits>::operator=(CAVL<Traits>&& another) noexcept{
    if (this == &another)
        return *this;
    std::lock(m_mtx, another.m_mtx);
    std::lock_guard<std::recursive_mutex> lock1(m_mtx, std::adopt_lock);
    std::lock_guard<std::recursive_mutex> lock2(another.m_mtx, std::adopt_lock);
    Destroy(m_pRoot);
    m_pRoot = std::exchange(another.m_pRoot,nullptr);
    return *this;

}
#endif // __AVL_H__