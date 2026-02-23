#ifndef __ARRAY_H__
#define __ARRAY_H__
#include <iostream>
#include <assert.h>
using namespace std;
#include <stddef.h>
#include "../algorithms/sorting.h"
#include "GeneralIterator.h"

template <typename _T>
struct Trait1
{
    using T = _T;
};

template <typename Container>
class ArrayForwardIterator : public GeneralIterator<Container>
{ 
  using Parent = GeneralIterator<Container>;
  public:
    ArrayForwardIterator(Container *pContainer, Size pos=0)       : Parent(pContainer, pos){}
    ArrayForwardIterator(ArrayForwardIterator<Container> &another):  Parent(another){}

    ArrayForwardIterator<Container> &operator++(){
        if( Parent::m_pos < Parent::m_pContainer->getSize() )
            ++Parent::m_pos;
        return *this;
    }
};

template <typename Container>
class ArrayBackwardIterator : public GeneralIterator<Container>
{ 
  using Parent = GeneralIterator<Container>;
  public:
    ArrayBackwardIterator(Container *pContainer, Size pos=0)          : Parent(pContainer, pos){}
    ArrayBackwardIterator(ArrayBackwardIterator<Container> &another)  :  Parent(another){}

    ArrayBackwardIterator<Container> &operator++(){
        if( Parent::m_pos > -1 )
            --Parent::m_pos;
        return *this;
    }
};

template <typename Traits>
class CArray {
    using value_type  = typename Traits::T;
    using  forward_iterator  = ArrayForwardIterator < CArray<Traits> >;
    friend forward_iterator;
    using  backward_iterator = ArrayBackwardIterator< CArray<Traits> >;
    friend backward_iterator;
    friend GeneralIterator< CArray<Traits> >;

    struct Node{
        value_type m_value;
        ref_type   m_ref;

        Node(){}
        Node( value_type _value, ref_type _ref = -1)
            : m_value(_value), m_ref(_ref){   }
        value_type  GetValue   () const { return m_value; }
        value_type &GetValueRef() { return m_value; }

        ref_type    GetRef     () const { return m_ref;   }
        ref_type   &GetRefRef  () { return m_ref;   }
        Node &operator=(const Node &another){
          m_value = another.GetValue();
          m_ref   = another.GetRef();
          return *this;
        }
        bool operator==(const Node &another) const
        { return m_value == another.GetValue();   }
        bool operator<(const Node &another) const
        { return m_value < another.GetValue();   }
    };
    //using  CompareFunc = Traits::CompareFunc
    using  CompareFunc = bool (*)(const Node &, const Node &);
  private:
    Size m_capacity = 0, m_last = 0;
    Node *m_data = nullptr;

  public:
    CArray(Size size);
    virtual ~CArray();

    void push_back(value_type value, ref_type ref);
    value_type &operator[](Size index);
    Size getSize() const
    {   return m_last + 1;  };
    void resize(Size delta = 10);
    void sort( CompareFunc pComp );

    forward_iterator begin()
    { return forward_iterator(this);  }
    forward_iterator end()
    { return forward_iterator(this, getSize());  }

    backward_iterator rbegin()
    { return backward_iterator(this, getSize()-1);  }
    backward_iterator rend()
    { return backward_iterator(this, -1);  }

    template <typename ObjFunc, typename ...Args>
    void Foreach(ObjFunc of, Args... args){
        ::Foreach(*this, of, args...);
        // for (auto i = 0; i < getSize(); ++i)
        //     of(m_data[i], args...);
    }
    template <typename ObjFunc, typename ...Args>
    auto FirstThat(ObjFunc of, Args... args){
        return ::FirstThat(*this, of, args...);
    }
    friend ostream &operator<<(ostream &os, CArray<Traits> &container){
        os << "CArray: size = " << container.getSize() << endl;
        os << "[";
        for (auto i = 0; i < container.getSize(); ++i)
          os << "(" << container.m_data[i].GetValue() << ":" << container.m_data[i].GetRef() << "),";
        os << "]" << endl;
        return os;
    }
};

template <typename Traits>
CArray<Traits>::CArray(Size size) {
  m_capacity = size;
  m_data = new Node[size];
}
template <typename Traits>
CArray<Traits>::~CArray() { delete[] m_data; }

template <typename Traits>
typename CArray<Traits>::value_type &CArray<Traits>::operator[](Size index) {
    // cout << "XResizing from " << m_capacity << " to at least " << index + 5 << endl;
    if (index > m_capacity) {
      cout << "Resizing from " << m_capacity << " to at least " << index + 5 << endl;
      resize(index - m_last + 5);
    }
    assert(index < m_capacity);
    if (index > m_last)
      m_last = index;
    return m_data[index].GetValueRef();
}

template <typename Traits>
void CArray<Traits>::push_back(value_type value, ref_type ref) {
    if (m_last >= m_capacity)
      resize();
    m_data[m_last++] = Node(value, ref);
}

template <typename Traits>
void CArray<Traits>::resize(Size delta) {
    Size new_capacity = m_capacity + delta;
    Node *new_data = new Node[new_capacity];
    for (auto i = 0; i < m_capacity; ++i)
      new_data[i] = m_data[i];
    delete[] m_data;
    m_data = new_data;
    m_capacity = new_capacity;
}

template <typename Traits>
void CArray<Traits>::sort( CompareFunc pComp ){
    BurbujaRecursivo(m_data, m_last+1, pComp);
}

// template <typename Traits>
// ostream &operator<<(ostream &os, CArray<Traits> &arr) {
//   os << "CArray: size = " << arr.getSize() << endl;
//   os << "[";
//   for (auto i = 0; i < arr.getSize(); ++i)
//     os << "(" << arr.m_data[i].GetValue() << ":" << arr.m_data[i].GetRef() << "),";
//   os << "]" << endl;
//   return os;
// }

void DemoArray();

#endif // __ARRAY_H__
