#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

#include <iostream>
#include <utility>
#include <mutex>
#include "../general/types.h"
#include "../util.h"
#include "treetraits.h"

//Forward declarations
template<typename Traits> class NodeBinaryTree;
template<typename Traits> class CBinaryTree;
template<typename Traits> class CBinaryTreeForwardIterator;
template<typename Traits> class CBinaryTreeBackwardIterator;

//Backward Iterator
template<typename Traits>
class CBinaryTreeBackwardIterator{
    using BackwardIterator = CBinaryTreeBackwardIterator<Traits>;
    using Node             = NodeBinaryTree<Traits>;
    using value_type       = typename Traits::value_type;
private:
    Node *m_pCurrent = nullptr;
public:
    CBinaryTreeBackwardIterator(Node *pCurrent)
        : m_pCurrent(pCurrent)
    {}
    value_type& operator*();
    bool operator!=(const BackwardIterator& another) const;
    bool operator==(const BackwardIterator& another) const;
    BackwardIterator& operator++();
};

//Funciones BackwardIterator
template <typename Traits>
typename CBinaryTreeBackwardIterator<Traits>::value_type& 
CBinaryTreeBackwardIterator<Traits>::operator*(){
    return m_pCurrent->GetValueRef();
}

template <typename Traits>
bool CBinaryTreeBackwardIterator<Traits>::operator!=(const CBinaryTreeBackwardIterator<Traits>& another) const{
    return  m_pCurrent != another.m_pCurrent;
}
template <typename Traits>
bool CBinaryTreeBackwardIterator<Traits>::operator==(const CBinaryTreeBackwardIterator<Traits>& another) const{
    return m_pCurrent == another.m_pCurrent;
}
template <typename Traits>
CBinaryTreeBackwardIterator<Traits>& CBinaryTreeBackwardIterator<Traits>::operator++(){
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
class CBinaryTreeForwardIterator{
    using ForwardIterator = CBinaryTreeForwardIterator<Traits>;
    using Node            = NodeBinaryTree<Traits>;
    using value_type      = typename Traits::value_type;
private:
    Node *m_pCurrent = nullptr;

public:
    CBinaryTreeForwardIterator(Node *pCurrent)
        : m_pCurrent(pCurrent)
    {}
    value_type& operator*();
    bool operator!=(const ForwardIterator& another) const;
    bool operator==(const ForwardIterator& another) const;
    ForwardIterator& operator++();

};

//Funciones ForwardIterator
template <typename Traits>
typename CBinaryTreeForwardIterator<Traits>::value_type& CBinaryTreeForwardIterator<Traits>::operator*(){ return m_pCurrent->GetValueRef(); }
template <typename Traits>
bool CBinaryTreeForwardIterator<Traits>::operator!=(const CBinaryTreeForwardIterator<Traits>& another) const { return m_pCurrent!=another.m_pCurrent; }
template <typename Traits>
bool CBinaryTreeForwardIterator<Traits>::operator==(const CBinaryTreeForwardIterator<Traits>& another) const { return m_pCurrent==another.m_pCurrent; }

template <typename Traits>
CBinaryTreeForwardIterator<Traits>& CBinaryTreeForwardIterator<Traits>::operator++() {
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
class NodeBinaryTree{
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeBinaryTree<Traits>;
    using  CompareFunc = typename Traits::CompareFunc;
private:
    value_type m_data;
    ref_type   m_ref;
    Node *m_pParent    = nullptr; 
    Node *m_pChild[2]  = { nullptr} ;
public:
    NodeBinaryTree(){}
    NodeBinaryTree( value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref){   }
    value_type  GetValue   () const { return m_data; }
    value_type &GetValueRef()       { return m_data; }
    ref_type    GetRef     () const { return m_ref;  }
    ref_type   &GetRefRef  ()       { return m_ref;  }

    friend class CBinaryTree<Traits>;
    friend class CBinaryTreeForwardIterator<Traits>;
    friend class CBinaryTreeBackwardIterator<Traits>;
};

template <typename Traits>
class CBinaryTree{
public:
    using  value_type         = typename Traits::value_type;
    using  Node               = NodeBinaryTree<Traits>;
    using  ForwardIterator    = CBinaryTreeForwardIterator<Traits>;
    using  CompareFunc        = typename Traits::CompareFunc;
    using  BackwardIterator   = CBinaryTreeBackwardIterator<Traits>;
private:
    Node *m_pRoot = nullptr;
    CompareFunc comp;
    mutable std::recursive_mutex m_mtx;

public:
    CBinaryTree(){}
    // TODO: Copy constructor
    CBinaryTree(const CBinaryTree &another);
    // TODO: Move constructor
    CBinaryTree(CBinaryTree &&another) noexcept {
        std::lock_guard<std::recursive_mutex> lock(another.m_mtx);
        m_pRoot = std::exchange(another.m_pRoot, nullptr);
    }
    virtual ~CBinaryTree(){
        Destroy(m_pRoot);
    }
private:
    void InternalInsert(Node *&rCurrent, Node *pParent, const value_type &val, ref_type ref){
        if( !rCurrent ){
            rCurrent = new Node(val, ref);
            rCurrent->m_pParent = pParent;
            return;
        }
        auto path = comp(val, rCurrent->GetValue());
        InternalInsert(rCurrent->m_pChild[path], rCurrent, val, ref);
         
    }

    Node* Clone(Node *pCurrent){
        if (!pCurrent)
            return nullptr;
        Node *pNewNode = new Node(pCurrent->GetValue(),pCurrent->GetRef());
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
    void InternalRemove(Node *&pCurrent, const value_type& value){
        if (!pCurrent)
            return;
        if ( value == pCurrent->GetValueRef() ){
            //caso 1 - nodo hoja
            if (pCurrent->m_pChild[0] == nullptr && pCurrent->m_pChild[1] == nullptr){
                delete pCurrent;
                pCurrent = nullptr;
                return;
            }
            //caso 2a - nodo tiene solo hijo derecho
            if (!pCurrent->m_pChild[0] && pCurrent->m_pChild[1]){
                Node* pChild      = pCurrent->m_pChild[1];
                pChild->m_pParent = pCurrent->m_pParent;
                delete pCurrent;
                pCurrent = pChild;
                return;
            }
            //caso 2b - nodo tiene solo hijo izquierdo
            if (pCurrent->m_pChild[0] && !pCurrent->m_pChild[1]){
                Node* pChild      = pCurrent->m_pChild[0];
                pChild->m_pParent = pCurrent->m_pParent;
                delete pCurrent;
                pCurrent = pChild;
                return;
            }
            //caso 3 - dos hijos
            Node* pSucc = FindMin(pCurrent->m_pChild[1]);
            pCurrent->GetValueRef() = pSucc->GetValue();
            pCurrent->GetRefRef()   = pSucc->GetRef();
            InternalRemove(pCurrent->m_pChild[1], pSucc->GetValue());
            return;
        }
        auto path = comp(value, pCurrent->GetValueRef() );
        InternalRemove(pCurrent->m_pChild[path], value);
    }

    //IMPRIMIR ARBOL
    void InternalPrintTree(Node* pNode, int depth) {
        if (!pNode) return;
        InternalPrintTree(pNode->m_pChild[0], depth + 1);
        for (int i = 0; i < depth; i++) std::cout << "\t";
        std::cout << "(" << pNode->GetValue() << ")\n";
        InternalPrintTree(pNode->m_pChild[1], depth + 1);
    }

    //Operator <<
    friend std::ostream& operator<<(std::ostream& os, CBinaryTree<Traits>& BinaryTree){
        std::lock_guard<std::recursive_mutex> lock(BinaryTree.m_mtx);
        os << "CBinaryTree" << std::endl;
        os << "[";
            bool first = true;
            for (auto it = BinaryTree.begin(); it != BinaryTree.end(); ++it){
                if (!first)
                    os << " -> ";
                os << *it;
                first = false;
            }
        os << "]";
        return os;
    }

    //Operator >>
    friend std::istream& operator>>(std::istream& is, CBinaryTree<Traits>& BinaryTree){
        std::lock_guard<std::recursive_mutex> lock(BinaryTree.m_mtx);
        size_t nElements;
        is >> nElements;
        for (size_t i = 0; i < nElements; ++i){
            value_type val;
            ref_type   ref;
            is >> val >> ref;
            BinaryTree.Insert(val, ref);
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

    CBinaryTree& operator=(const CBinaryTree& another);
    CBinaryTree& operator=(CBinaryTree&& another) noexcept;
    ForwardIterator  begin();
    ForwardIterator  end();
    BackwardIterator rbegin();
    BackwardIterator rend();
};


//ITERADORES - ForwardIterator
template <typename Traits>
typename CBinaryTree<Traits>::ForwardIterator CBinaryTree<Traits>::begin(){
    if (!m_pRoot)
        return ForwardIterator(nullptr);
    Node *pCurrent = m_pRoot;
    while (pCurrent->m_pChild[0])
        pCurrent = pCurrent->m_pChild[0];
    return ForwardIterator(pCurrent); 
}

template <typename Traits>
typename CBinaryTree<Traits>::ForwardIterator 
CBinaryTree<Traits>::end(){ return ForwardIterator(nullptr); }

//ITERADORES - BackwardIterator
template <typename Traits>
typename CBinaryTree<Traits>::BackwardIterator 
CBinaryTree<Traits>::rbegin() {
    if (!m_pRoot)
        return BackwardIterator(nullptr);
    Node *pCurrent = m_pRoot;
    while (pCurrent->m_pChild[1])
        pCurrent = pCurrent->m_pChild[1];
    return BackwardIterator(pCurrent);
}
template <typename Traits>
typename CBinaryTree<Traits>::BackwardIterator 
CBinaryTree<Traits>::rend(){ return BackwardIterator(nullptr); }

//Copy Constructor
template <typename Traits>
CBinaryTree<Traits>::CBinaryTree(const CBinaryTree<Traits> &another){
    std::lock_guard<std::recursive_mutex> lock(another.m_mtx);
    m_pRoot = Clone(another.m_pRoot);
}

//Operador de asignación '=' para copy constructor
template <typename Traits>
CBinaryTree<Traits>& CBinaryTree<Traits>::operator=(const CBinaryTree& another){
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
CBinaryTree<Traits>& CBinaryTree<Traits>::operator=(CBinaryTree<Traits>&& another) noexcept{
    if (this == &another)
        return *this;
    std::lock(m_mtx, another.m_mtx);
    std::lock_guard<std::recursive_mutex> lock1(m_mtx, std::adopt_lock);
    std::lock_guard<std::recursive_mutex> lock2(another.m_mtx, std::adopt_lock);
    Destroy(m_pRoot);
    m_pRoot = std::exchange(another.m_pRoot,nullptr);
    return *this;

}
#endif // __BINARYTREE_H__