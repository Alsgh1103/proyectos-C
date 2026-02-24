#ifndef __BINARYTREEAVL__
#define __BINARYTREEAVL__

#include "binarytree.h"

template <typename Traits>
class CBinaryTreeAVL : public CBinaryTree<Traits> {
    using Node = typename CBinaryTree<Traits>::Node;
    using value_type = typename Traits::value_type;

public:
    CBinaryTreeAVL() : CBinaryTree<Traits>() {}

    void Insert(const value_type &val, ref_type ref) {
        std::lock_guard<std::recursive_mutex> lock(this->m_mtx);
        InternalInsertAVL(this->m_pRoot, nullptr, val, ref);
    }

    void Remove(const value_type &val) {
        std::lock_guard<std::recursive_mutex> lock(this->m_mtx);
        InternalRemoveAVL(this->m_pRoot, val);
    }

    CBinaryTreeAVL(CBinaryTree<Traits> &baseTree) : CBinaryTree<Traits>() {
        for (auto it = baseTree.begin(); it != baseTree.end(); ++it) {
            this->Insert(*it, -1); 
    }
}

protected:

    Size GetNodeHeight(Node* pNode) {
        return pNode ? pNode->m_height : 0;
    }

    Size GetBalanceFactor(Node* pNode) {
        if (!pNode) return 0;
        return GetNodeHeight(pNode->GetChild(0)) - GetNodeHeight(pNode->GetChild(1));
    }

    void UpdateHeight(Node* pNode) {
        if (pNode) {
            Size leftHeight  = GetNodeHeight(pNode->GetChild(0));
            Size rightHeight = GetNodeHeight(pNode->GetChild(1));
            pNode->m_height = 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
        }
    }

    void RotateRight(Node*& pSubtreeRoot) {
        Node* pLeftChild = pSubtreeRoot->m_pChild[0];
        Node* pSubtreeOfLeft = pLeftChild->m_pChild[1];

        pLeftChild->m_pChild[1] = pSubtreeRoot;
        pSubtreeRoot->m_pChild[0] = pSubtreeOfLeft;

        pLeftChild->m_pParent = pSubtreeRoot->m_pParent;
        pSubtreeRoot->m_pParent = pLeftChild;
        if (pSubtreeOfLeft) pSubtreeOfLeft->m_pParent = pSubtreeRoot;

        UpdateHeight(pSubtreeRoot);
        UpdateHeight(pLeftChild);

        pSubtreeRoot = pLeftChild;
    }

    void RotateLeft(Node*& pSubtreeRoot) {
        Node* pRightChild = pSubtreeRoot->m_pChild[1];
        Node* pSubtreeOfRight = pRightChild->m_pChild[0];

        pRightChild->m_pChild[0] = pSubtreeRoot;
        pSubtreeRoot->m_pChild[1] = pSubtreeOfRight;

        pRightChild->m_pParent = pSubtreeRoot->m_pParent;
        pSubtreeRoot->m_pParent = pRightChild;
        if (pSubtreeOfRight) pSubtreeOfRight->m_pParent = pSubtreeRoot;

        UpdateHeight(pSubtreeRoot);
        UpdateHeight(pRightChild);

        pSubtreeRoot = pRightChild;
    }

    void ApplyBalance(Node*& pNode) {
        if (!pNode) return;

        UpdateHeight(pNode);
        Size balance = GetBalanceFactor(pNode);
        if (balance > 1) {
            if (GetBalanceFactor(pNode->m_pChild[0]) < 0)
                RotateLeft(pNode->m_pChild[0]);
            RotateRight(pNode);
        }
        else if (balance < -1) {
            if (GetBalanceFactor(pNode->m_pChild[1]) > 0)
                RotateRight(pNode->m_pChild[1]);
            RotateLeft(pNode);
        }
    }

    void InternalInsertAVL(Node*& pCurrent, Node* pParent, const value_type &val, ref_type ref) {
        if (!pCurrent) {
            pCurrent = new Node(val, ref, pParent);
            return;
        }

        bool side = this->comp(val, pCurrent->GetValue());
        InternalInsertAVL(pCurrent->m_pChild[side], pCurrent, val, ref);

        ApplyBalance(pCurrent);
    }

    void InternalRemoveAVL(Node*& pCurrent, const value_type &val) {
        if (!pCurrent) return;

        if (this->comp(val, pCurrent->GetValue()))
            InternalRemoveAVL(pCurrent->m_pChild[1], val);
        else if (this->comp(pCurrent->GetValue(), val))
            InternalRemoveAVL(pCurrent->m_pChild[0], val);
        else {
            if (pCurrent->m_pChild[0] && pCurrent->m_pChild[1]) {
                Node* pSuccessor = pCurrent->m_pChild[1];
                while (pSuccessor->m_pChild[0]) pSuccessor = pSuccessor->m_pChild[0];
                
                pCurrent->GetValueRef() = pSuccessor->GetValue();
                pCurrent->GetRefRef()   = pSuccessor->GetRef();
                InternalRemoveAVL(pCurrent->m_pChild[1], pSuccessor->GetValue());
            } else {
                Node* pTrash = pCurrent;
                Node* pChild = pCurrent->m_pChild[0] ? pCurrent->m_pChild[0] : pCurrent->m_pChild[1];
                pCurrent = pChild;
                if (pCurrent) pCurrent->m_pParent = pTrash->m_pParent;
                delete pTrash;
            }
        }

        if (pCurrent) ApplyBalance(pCurrent);
    }
};

#endif //__BINARYTREEAVL__