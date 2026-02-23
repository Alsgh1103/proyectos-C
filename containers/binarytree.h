#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

#include <iostream>
#include <mutex>
#include <utility>
#include <fstream>
#include <string>
#include "../general/types.h"
#include "../util.h"
#include "GeneralIterator.h"

using namespace std;

template <typename T>
struct TreeTraitAscending {
    using value_type  = T;
    using CompareFunc = greater<T>;
};

template <typename T>
struct TreeTraitDescending {
    using value_type  = T;
    using CompareFunc = less<T>;
};

template <typename Traits> class CBinaryTree;
template <typename Traits> class CBinaryTreeAVL;
template <typename Traits>
class NodeBinaryTree{
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeBinaryTree<Traits>;
    using  CompareFunc = typename Traits::CompareFunc;

    friend class CBinaryTree<Traits>;
    friend class CBinaryTreeAVL<Traits>;

protected:
    value_type m_data;
    ref_type   m_ref;
    Node *m_pChild[2];
    Node* m_pParent;
    Size m_height = 1;

public:
    NodeBinaryTree() : m_pChild{nullptr, nullptr}, m_pParent(nullptr) { }
    NodeBinaryTree( value_type _value, ref_type _ref = -1, Node* _parent = nullptr)
        : m_data(_value), m_ref(_ref), m_pChild{nullptr, nullptr}, m_pParent(_parent){ }
    
    value_type  GetValue   () const { return m_data; }
    value_type &GetValueRef()       { return m_data; }
    ref_type    GetRef     () const { return m_ref; }
    ref_type   &GetRefRef  ()       { return m_ref; }

    Node* GetChild(size_t index) const { return m_pChild[index]; }
    Node* GetParent() const            { return m_pParent; }
};

template <typename Container>
class BinaryTreeForwardIterator : public GeneralIterator<Container> {
    using Node = typename Container::Node;
    using Parent = GeneralIterator<Container>;

public:
    BinaryTreeForwardIterator(Container* pContainer, Node* pNode) 
        : Parent(pContainer, 0) { this->m_data = pNode; }

    BinaryTreeForwardIterator& operator++() {
        Node* pNode = this->m_data;
        if (!pNode) return *this;

        if (pNode->GetChild(1)) {
            pNode = pNode->GetChild(1);
            while (pNode->GetChild(0)) {
                pNode = pNode->GetChild(0);
            }
            this->m_data = pNode;
        }
        else {
            Node* pParent = pNode->GetParent();
            while (pParent && pNode == pParent->GetChild(1)) {
                pNode = pParent;
                pParent = pParent->GetParent();
            }
            this->m_data = pParent;
        }
        return *this;
    }

    typename Container::value_type& operator*() {
        return this->m_data->GetValueRef();
    }

    bool operator!=(const BinaryTreeForwardIterator<Container>& another) const {
        return this->m_data != another.m_data;
    }
};

template <typename Container>
class BinaryTreeBackwardIterator : public GeneralIterator<Container> {
    using Node = typename Container::Node;
    using Parent = GeneralIterator<Container>;

public:
    BinaryTreeBackwardIterator(Container* pContainer, Node* pNode)
        : Parent(pContainer, 0){ this->m_data = pNode; }

    BinaryTreeBackwardIterator& operator++() {
        Node* pNode = this->m_data;
        if (!pNode) return *this;

        if (pNode->GetChild(0)) {
            pNode = pNode->GetChild(0);
            while (pNode->GetChild(1)) {
                pNode = pNode->GetChild(1);
            }
            this->m_data = pNode;
        }
        else {
            Node* pParent = pNode->GetParent();
            while (pParent && pNode == pParent->GetChild(0)) {
                pNode = pParent;
                pParent = pParent->GetParent();
            }
            this->m_data = pParent;
        }
        return *this;
    }
    
    typename Container::value_type& operator*() {
        return this->m_data->GetValueRef();
    }

    bool operator!=(const BinaryTreeBackwardIterator<Container>& another) const {
        return this->m_data != another.m_data;
    }
};

template <typename Traits>
class CBinaryTree{
public:
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeBinaryTree<Traits>;
    using  CompareFunc = typename Traits::CompareFunc;

    using forward_iterator  = BinaryTreeForwardIterator<CBinaryTree<Traits>>;
    using backward_iterator = BinaryTreeBackwardIterator<CBinaryTree<Traits>>;

    friend forward_iterator;
    friend backward_iterator;
    friend GeneralIterator<CBinaryTree<Traits>>;

protected:
    Node *m_pRoot = nullptr;
    CompareFunc comp;
    mutable mutex m_mutex;

public:
    CBinaryTree() : m_pRoot(nullptr) {}
    CBinaryTree(const CBinaryTree<Traits> &another);
    CBinaryTree(CBinaryTree<Traits> &&another) noexcept;
    virtual ~CBinaryTree();

    forward_iterator begin() { 
        lock_guard<mutex> lock(m_mutex);
        Node* pMin = m_pRoot;
        if(pMin) {
            while(pMin->GetChild(0)) pMin = pMin->GetChild(0);
        }
        return forward_iterator(this, pMin); 
    }

    forward_iterator end() { 
        return forward_iterator(this, nullptr); 
    }

    backward_iterator rbegin() { 
        lock_guard<mutex> lock(m_mutex);
        Node* pMax = m_pRoot;
        if(pMax) {
            while(pMax->GetChild(1)) pMax = pMax->GetChild(1);
        }
        return backward_iterator(this, pMax); 
    }

    backward_iterator rend() { 
        return backward_iterator(this, nullptr); 
    }

    void Insert(const value_type &val, ref_type ref){
        lock_guard<mutex> lock(m_mutex);
        InternalInsert(m_pRoot, nullptr, val, ref);
    }

    void Remove(const value_type &val) {
        lock_guard<mutex> lock(m_mutex);
        InternalRemove(m_pRoot, val);
    }

    // Recorrido Pre-Order
    template <typename Func, typename ...Args>
    void PreOrder(Func fn, Args&&... args) {
        lock_guard<mutex> lock(m_mutex);
        InternalPreOrder(m_pRoot, fn, forward<Args>(args)...);
    }

    // Recorrido In-Order
    template <typename Func, typename ...Args>
    void InOrder(Func fn, Args&&... args) {
        lock_guard<mutex> lock(m_mutex);
        InternalInOrder(m_pRoot, fn, forward<Args>(args)...);
    }

    // Recorrido Post-Order
    template <typename Func, typename ...Args>
    void PostOrder(Func fn, Args&&... args) {
        lock_guard<mutex> lock(m_mutex);
        InternalPostOrder(m_pRoot, fn, forward<Args>(args)...);
    }

    template <typename Func, typename ...Args>
    void Foreach(Func fn, Args&&... args) {
        lock_guard<mutex> lock(m_mutex);
        InternalInOrder(m_pRoot, fn, forward<Args>(args)...);
    }

    template <typename Func, typename ...Args>
    forward_iterator FirstThat(Func fn, Args&&... args) {
        lock_guard<mutex> lock(m_mutex);
        Node* pFound = InternalFirstThat(m_pRoot, fn, forward<Args>(args)...);
        
        if (pFound) 
            return forward_iterator(this, pFound);
        return end();
    }

    friend ostream& operator<<(ostream &os, CBinaryTree<Traits> &tree) {
        lock_guard<mutex> lock(tree.m_mutex);
        os << "[ ";
        tree.InternalPrintFlat(tree.m_pRoot, os);
        os << "]";
        return os;
    }

    friend istream& operator>>(istream &is, CBinaryTree<Traits> &tree) {
        lock_guard<mutex> lock(tree.m_mutex);
        tree.DeleteTreeRecursive(tree.m_pRoot);
        tree.m_pRoot = nullptr;

        char c;
        typename Traits::value_type val;
        ref_type ref;

        while (is >> c && c != '[') {}
        while (is >> c && c != ']') {
            if (c == '(') {
                is >> val;
                is >> c;
                if(c == ':') { 
                    is >> ref;
                    tree.InternalInsert(tree.m_pRoot, nullptr, val, ref);
                }
                while(is >> c && c != ',' && c != ']' && c != ')');
                if (c == ']') break;
            }
        }
        return is;
    }

    void Print() {
        lock_guard<mutex> lock(m_mutex);
        if (!m_pRoot) cout << "(Arbol Vacio)" << endl;
        else InternalPrintTree(m_pRoot, 0);
    }

private:
    void InternalInsert(Node *&rCurrent, Node* pParent, const value_type &val, ref_type ref){
        if( !rCurrent ){
            rCurrent = new Node(val, ref, pParent);
            return;
        }
        bool side = comp(val, rCurrent->GetValue());
        InternalInsert(rCurrent->m_pChild[side], rCurrent, val, ref);
    }

    void InternalRemove(Node*& pCurrent, const value_type &val) {
        if (!pCurrent) return;
        if (comp(val, pCurrent->GetValue())) 
            InternalRemove(pCurrent->m_pChild[1], val);
        else if (comp(pCurrent->GetValue(), val)) 
            InternalRemove(pCurrent->m_pChild[0], val);
        else {
            if (pCurrent->m_pChild[0] && pCurrent->m_pChild[1]) {
                Node* pSuccessor = pCurrent->m_pChild[1];
                while (pSuccessor->m_pChild[0]) {
                    pSuccessor = pSuccessor->m_pChild[0];
                }
                pCurrent->GetValueRef() = pSuccessor->GetValue();
                pCurrent->GetRefRef()   = pSuccessor->GetRef();
                InternalRemove(pCurrent->m_pChild[1], pSuccessor->GetValue());
            }
            else {
                Node* pTrash = pCurrent;
                Node* pChild = pCurrent->m_pChild[0] ? pCurrent->m_pChild[0] : pCurrent->m_pChild[1];
                pCurrent = pChild;

                if (pCurrent) 
                    pCurrent->m_pParent = pTrash->m_pParent;
                delete pTrash;
            }
        }
    }

    void InternalPrintFlat(Node* pNode, ostream &os) {
        if (!pNode) return;
        InternalPrintFlat(pNode->GetChild(0), os);
        os << "(" << pNode->GetValue() << ":" << pNode->GetRef() << "), ";
        InternalPrintFlat(pNode->GetChild(1), os);
    }

    void InternalPrintTree(Node* pNode, Size level) {
        if (!pNode) return;
    
        InternalPrintTree(pNode->GetChild(1), level + 1);
        
        for (auto i = 0; i < level; ++i) cout << "\t";
        if (level > 0) cout << " |--"; 
        
        cout << pNode->GetValue() << endl;
        InternalPrintTree(pNode->GetChild(0), level + 1);
    }

    template <typename Func, typename ...Args>
    void InternalPreOrder(Node* pNode, Func fn, Args... args) {
        if (!pNode) return;
        fn(pNode->GetValueRef(), args...);
        InternalPreOrder(pNode->GetChild(0), fn, args...);
        InternalPreOrder(pNode->GetChild(1), fn, args...);
    }

    template <typename Func, typename ...Args>
    void InternalInOrder(Node* pNode, Func fn, Args... args) {
        if (!pNode) return;
        InternalInOrder(pNode->GetChild(0), fn, args...);
        fn(pNode->GetValueRef(), args...);
        InternalInOrder(pNode->GetChild(1), fn, args...);
    }

    template <typename Func, typename ...Args>
    void InternalPostOrder(Node* pNode, Func fn, Args... args) {
        if (!pNode) return;
        InternalPostOrder(pNode->GetChild(0), fn, args...);
        InternalPostOrder(pNode->GetChild(1), fn, args...);
        fn(pNode->GetValueRef(), args...); 
    }

    template <typename Func, typename ...Args>
    Node* InternalFirstThat(Node* pNode, Func fn, Args... args) {
        if (!pNode) return nullptr;
        Node* pLeft = InternalFirstThat(pNode->GetChild(0), fn, args...);
        if (pLeft) return pLeft;
        if (fn(pNode->GetValueRef(), args...)) 
            return pNode;
        return InternalFirstThat(pNode->GetChild(1), fn, args...);
    }

    // Helper Recursivo para Copiar
    Node* CloneNodeRecursive(Node* pNodeToCopy, Node* pNewParent) {
        if (!pNodeToCopy) return nullptr;
        
        Node* pNewNode = new Node(pNodeToCopy->GetValue(), pNodeToCopy->GetRef(), pNewParent);
        pNewNode->m_pChild[0] = CloneNodeRecursive(pNodeToCopy->m_pChild[0], pNewNode);
        pNewNode->m_pChild[1] = CloneNodeRecursive(pNodeToCopy->m_pChild[1], pNewNode);
        
        return pNewNode;
    }

    // Helper Recursivo para Borrar
    void DeleteTreeRecursive(Node* pNodeToDelete) {
        if (!pNodeToDelete) return;
        
        DeleteTreeRecursive(pNodeToDelete->m_pChild[0]);
        DeleteTreeRecursive(pNodeToDelete->m_pChild[1]);
        
        delete pNodeToDelete;
    }
};

// Copy constructor
template <typename Traits>
CBinaryTree<Traits>::CBinaryTree(const CBinaryTree<Traits> &another){
    lock_guard<mutex> lock(another.m_mutex);
    m_pRoot = CloneNodeRecursive(another.m_pRoot, nullptr);
}

// Move constructor
template <typename Traits>
CBinaryTree<Traits>::CBinaryTree(CBinaryTree<Traits> &&another) noexcept{
    lock_guard<mutex> lock(another.m_mutex);
    m_pRoot = exchange(another.m_pRoot, nullptr);
}

//Safe destructor
template <typename Traits>
CBinaryTree<Traits>::~CBinaryTree() {
    lock_guard<mutex> lock(m_mutex);
    DeleteTreeRecursive(m_pRoot);
    m_pRoot = nullptr;
}

void DemoBinaryTree();

#endif // __BINARYTREE_H__