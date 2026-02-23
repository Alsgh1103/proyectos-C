#ifndef CBTreePage_H
#define CBTreePage_H

#include <vector>
#include <iostream>
#include <cassert>
#include <functional>
#include <mutex>

// Forward declarations
template <typename Traits> class CBTree;
template <typename Traits> class CBTreeForwardIterator;
template <typename Traits> class CBTreeBackwardIterator;

// Encapsulación BTree Ascendente
template <typename _T, typename _ObjIDType = long, int _order = 3>
struct CBTreeTraitsAsc{

        using value_type           = _T;
        using ObjIDType            = _ObjIDType;
        using CompareFunc          = std::less<_T>;
        static constexpr int order = _order;
};

// Encapsulación BTree Descendente
template <typename _T, typename _ObjIDType = long, int _order = 3>
struct CBTreeTraitsDesc{
        using value_type            = _T;
        using ObjIDType             = _ObjIDType;
        using CompareFunc           = std::greater<_T>;
        static constexpr int order  = _order;
};

using namespace std;
enum bt_ErrorCode {
        bt_ok, 
        bt_overflow, 
        bt_underflow, 
        bt_duplicate, 
        bt_nofound, 
        bt_rootmerged};

template <typename Traits>
struct CBTreeEntry{

        using value_type = typename Traits::value_type;
        using ObjIDType  = typename Traits::ObjIDType;
        
        value_type              key;          //LLAVE DE BÚSQUEDA
        ObjIDType               ObjID;        //ID AL OBJETO ASOCIADO
        long                    UseCounter;   //CUANTAS VECES SE BUSCÓ

        CBTreeEntry(const value_type& _key, ObjIDType _ObjID)
                : key(_key), ObjID(_ObjID), UseCounter(0) 
        {}
        CBTreeEntry()
        {}
        operator value_type   () { return key; }
        long     GetUseCounter() { return UseCounter; }
};


template <typename Traits>
class CBTreePage
// this is the in-memory version of the CBTreePage
{
        friend class CBTree<Traits>;
        friend class CBTreeForwardIterator<Traits>;
        friend class CBTreeBackwardIterator<Traits>;
        using BTPage     = CBTreePage<Traits>;
        using ObjectInfo = CBTreeEntry<Traits>;
        using value_type = typename Traits::value_type;
        using ObjIDType  = typename Traits::ObjIDType;

public:
        CBTreePage(int maxKeys, bool unique = true);
        CBTreePage(CBTreePage&& other) noexcept;
        virtual ~CBTreePage();

        bt_ErrorCode  Insert(const value_type& key, ObjIDType ObjID);
        bt_ErrorCode  Remove(const value_type& key, ObjIDType ObjID);
        ObjectInfo*   Search(const value_type& key);
        void          Print(ostream& os);

        template<typename Func, typename... Args>
        void Inorden(Func fn, int level, Args&&... args);

        template<typename Func, typename... Args>
        void Preorden(Func fn, int level, Args&&... args);

        template<typename Func, typename... Args>
        void Postorden(Func fn, int level, Args&&... args);

        template<typename Func, typename... Args>
        void Foreach(Func fn, int level, Args&&... args);

        template<typename Func, typename... Args>
        ObjectInfo* FirstThat(Func fn, int level, Args&&... args);

protected:
       int  m_MinKeys;                   // Mínimo número de claves en un nodo
       int  m_MaxKeys,                   // Máximo número de claves en un nodo raíz
                m_MaxKeysForChilds;      // Máximo número de claves en los nodos hijos
       bool m_Unique;                    // Para saber si se acepta claves duplicadas
       bool m_isRoot;                    // 
       vector<ObjectInfo> m_Keys;        // Vector donde se guardan las claves de una página
       vector<BTPage *>m_SubPages;       // Vectir con las direcciones de memoria a subpáginas (HIJOS)
       int  m_KeyCount;                  // Cuantas claves hay actualmente
       void  Create();
       void  Reset ();
       void  Destroy () { Reset(); delete this; }
       void  clear ();

       bool  Redistribute1   (int &pos);
       bool  Redistribute2   (int pos);
       void  RedistributeR2L (int pos);
       void  RedistributeL2R (int pos);

       bool    TreatUnderflow  (int &pos)
       {       return Redistribute1(pos) || Redistribute2(pos);}

       bt_ErrorCode    Merge  (int pos);
       bt_ErrorCode    MergeRoot ();
       void  SplitChild (int pos);

       ObjectInfo &GetFirstObjectInfo();

       bool Overflow()  { return m_KeyCount > m_MaxKeys; }
       bool Underflow() { return m_KeyCount < MinNumberOfKeys(); }
       bool IsFull()    { return m_KeyCount >= m_MaxKeys; }
       int  MinNumberOfKeys()  { return 2*m_MaxKeys/3.0; }
       int  GetFreeCells()  { return m_MaxKeys - m_KeyCount; }
       int& NumberOfKeys()  { return m_KeyCount; }
       int  GetNumberOfKeys()  { return m_KeyCount; }
       bool IsRoot()  { return m_MaxKeysForChilds != m_MaxKeys; }
       void SetMaxKeysForChilds(int orderforchilds)
       {
               m_MaxKeysForChilds = orderforchilds;
       }

       int GetFreeCellsOnLeft(int pos);
       int GetFreeCellsOnRight(int pos);

private:
       bool SplitRoot();
       void SplitPageInto3(vector<ObjectInfo>   & tmpKeys,
                                               vector<BTPage *>  & SubPages,
                                               BTPage           *& pChild1,
                                               BTPage           *& pChild2,
                                               BTPage           *& pChild3,
                                               ObjectInfo        & oi1,
                                               ObjectInfo        & oi2);
       void MovePage(BTPage *  pChildPage,vector<ObjectInfo> & tmpKeys,vector<BTPage *> & tmpSubPages);
};

template <typename Container, typename ObjType>
int binary_search(Container& container, int first, int last, const ObjType& object)
{
       if( first >= last )
               return first;
       while( first < last )
       {
               int mid = (first+last)/2;
               if( object == (ObjType)container[mid ] )
                       return mid;
               if( object > (ObjType)container[mid ] )
                       first = mid+1;
               else
                       last  = mid;
       }
       if( object <= (ObjType)container[first] )
               return first;
       return last;
}

template <typename Container, typename ObjType>
void insert_at(Container& container, const ObjType &object, int pos)
{
       int size = container.size();
       for(int i = size-2 ; i >= pos ; i--)
               container[i+1] = container[i];
       container[pos] =  object;
		
}

template <typename Container>
void remove(Container& container, int pos)
{
       int size = container.size();
       for(int i = pos+1 ; i < size ; i++)
               container[i-1] = container[i];
}

template <typename Traits>
CBTreePage<Traits>:: CBTreePage(int maxKeys, bool unique)
                                       : m_MaxKeys(maxKeys), m_Unique(unique), m_KeyCount(0)
{
       Create();
       SetMaxKeysForChilds(m_MaxKeys);
}

template <typename Traits>
CBTreePage<Traits>::CBTreePage(CBTreePage&& other) noexcept
        : m_MinKeys          (other.m_MinKeys),
          m_MaxKeys          (other.m_MaxKeys),
          m_MaxKeysForChilds (other.m_MaxKeysForChilds),
          m_Unique           (other.m_Unique),
          m_isRoot           (other.m_isRoot),
          m_Keys             (std::move(other.m_Keys)),
          m_SubPages         (std::move(other.m_SubPages)),
          m_KeyCount         (std::exchange(other.m_KeyCount, 0))
{}

template <typename Traits>
CBTreePage<Traits>::~CBTreePage()
{
       Reset();
}

template <typename Traits>
bt_ErrorCode CBTreePage<Traits>::Insert(const value_type& key, ObjIDType ObjID)
{
        int pos = binary_search(m_Keys, 0, m_KeyCount, key);
        bt_ErrorCode error = bt_ok;

       if( pos < m_KeyCount && (value_type)m_Keys[pos] == key && m_Unique)
               return bt_duplicate; // this key is duplicate

       if( !m_SubPages[pos] ) // this is a leave
        {
                ::insert_at(m_Keys, ObjectInfo(key, ObjID), pos);
                m_KeyCount++;
               if( Overflow() )
                       return bt_overflow;
                return bt_ok;
        }
        else
        {
               // recursive insertion
                error = m_SubPages[pos]->Insert(key, ObjID);
                if( error == bt_overflow )
                {
                        if( !Redistribute1(pos) )
                                SplitChild(pos);
                       if( Overflow() )          // Propagate overflow
                               return bt_overflow;
                        return bt_ok;
                }
        }
       if( Overflow() ) // node overflow
               return bt_overflow;
        return bt_ok;
}

template <typename Traits>
bool CBTreePage<Traits>::Redistribute1(int &pos)
{
       if( m_SubPages[pos]->Underflow() )
       {
               int nkol = 0,
                       nkor = 0;
               // is this the first element or there are more elements on right brother
               if( pos > 0 )
                       nkol = m_SubPages[pos-1]->NumberOfKeys();
               if( pos < NumberOfKeys() )
                       nkor = m_SubPages[pos+1]->NumberOfKeys();

               if( nkol > nkor )
                       if( m_SubPages[pos-1]->NumberOfKeys() > m_SubPages[pos-1]->MinNumberOfKeys() )
                               RedistributeL2R(pos-1); // bring elements from left brother
                       else
                               if( pos == NumberOfKeys() )
                                       return (--pos, false);
                               else
                                       return false;
               else //nkol < nkor )
                       if( m_SubPages[pos+1]->NumberOfKeys() > m_SubPages[pos+1]->MinNumberOfKeys() )
                               RedistributeR2L(pos+1); // bring elements from right brother
                       else
                               if( pos == 0 )
                                       return (++pos, false);
                               else
                                       return false;
       }
       else // it is due to overflow
       {
               int fcol = GetFreeCellsOnLeft(pos),   // Free Cells On Left
               fcor = GetFreeCellsOnRight(pos);  // Free Cells On Right

               if( !fcol && !fcor && m_SubPages[pos]->IsFull() )
                       return false;
               if( fcol > fcor ) // There is more space on left
                       RedistributeR2L(pos);
               else
                       RedistributeL2R(pos);

       }
       return true;
}

// Redistribute2 function
// it considers two brothers m_SubPages[pos-1] && m_SubPages[pos+1]
// if it fails the only way is merge !
template <typename Traits>
bool CBTreePage<Traits>::Redistribute2(int pos)
{
       assert( pos > 0 && pos < NumberOfKeys()  );
       assert( m_SubPages[pos-1] != 0 && m_SubPages[pos] != 0 && m_SubPages[pos+1] != 0 );
       assert( m_SubPages[pos-1]->Underflow() ||
                       m_SubPages[ pos ]->Underflow() ||
                       m_SubPages[pos+1]->Underflow() );

       if( m_SubPages[pos-1]->Underflow() )
       {       // Rotate R2L
               RedistributeR2L(pos+1);
               RedistributeR2L(pos);
               if( m_SubPages[pos-1]->Underflow() )
                       return false;
       }
       else if( m_SubPages[pos+1]->Underflow() )
       {       // Rotate L2R
               RedistributeL2R(pos-1);
               RedistributeL2R(pos);
               if( m_SubPages[pos+1]->Underflow() )
                       return false;
       }
       else // The problem is exactly at pos !
       {
               // Rotate L2R
               RedistributeL2R(pos-1);
               RedistributeR2L(pos+1);
               if( m_SubPages[pos]->Underflow() )
                       return false;
       }
       return true;
}

template <typename Traits>
void CBTreePage<Traits>::RedistributeR2L(int pos)  
{
       BTPage  *pSource = m_SubPages[ pos ],
                       *pTarget = m_SubPages[pos-1];

       while(pSource->GetNumberOfKeys() > pSource->MinNumberOfKeys() &&
                 pTarget->GetNumberOfKeys() < pSource->GetNumberOfKeys() )
       {
               // Move from this page to the down-left page \/
               ::insert_at(pTarget->m_Keys, m_Keys[pos-1], pTarget->NumberOfKeys()++);
               // Move the pointer leftest pointer to the rightest position
               ::insert_at(pTarget->m_SubPages, pSource->m_SubPages[0], pTarget->NumberOfKeys());

               // Move the leftest element to the root
               m_Keys[pos-1] = pSource->m_Keys[0];

               // Remove the leftest element from rigth page
               ::remove(pSource->m_Keys    , 0);
               ::remove(pSource->m_SubPages, 0);
               pSource->NumberOfKeys()--;
       }
}

template <typename Traits>
void CBTreePage<Traits>::RedistributeL2R(int pos)
{
       BTPage  *pSource = m_SubPages[pos],
                       *pTarget = m_SubPages[pos+1];
       while(pSource->GetNumberOfKeys() > pSource->MinNumberOfKeys() &&
                 pTarget->GetNumberOfKeys() < pSource->GetNumberOfKeys() )
       {
               // Move from this page to the down-RIGHT page \/
               ::insert_at(pTarget->m_Keys, m_Keys[pos], 0);
               // Move the pointer rightest pointer to the leftest position
               ::insert_at(pTarget->m_SubPages, pSource->m_SubPages[pSource->NumberOfKeys()], 0);
               pTarget->NumberOfKeys()++;

               // Move the rightest element to the root
               m_Keys[pos] = pSource->m_Keys[pSource->NumberOfKeys()-1];

               // Remove the leftest element from rigth page
               // it is not necessary erase because m_KeyCount controls
               pSource->NumberOfKeys()--;
       }
}

template <typename Traits>
void CBTreePage<Traits>::SplitChild(int pos)
{
       // FIRST: deciding the second page to split
       BTPage  *pChild1 = 0, *pChild2 = 0;
       if( pos > 0 )                                   // is left page full ?
               if( m_SubPages[pos-1]->IsFull() )
               {
                       pChild1 = m_SubPages[pos-1];
                       pChild2 = m_SubPages[pos--];
               }
       if( pos < GetNumberOfKeys() )   // is right page full ?
               if( m_SubPages[pos+1]->IsFull() )
               {
                       pChild1 = m_SubPages[pos];
                       pChild2 = m_SubPages[pos+1];
               }

        // int nKeys = pChild1->GetNumberOfKeys() + pChild2->GetNumberOfKeys() + 1;

       // SECOND: copy both pages to a temporal one
       // Create two tmp vector
       vector<ObjectInfo> tmpKeys;
       //tmpKeys.resize(nKeys);
       vector<BTPage *>   tmpSubPages;
       //tmpKeys.resize(nKeys+1);

       // copy from left child
       MovePage(pChild1, tmpKeys, tmpSubPages);
       // copy a key from parent
       tmpKeys    .push_back(m_Keys[pos]);

       // copy from right child
       MovePage(pChild2, tmpKeys, tmpSubPages);

       BTPage *pChild3 = 0;
       ObjectInfo oi1, oi2;
       SplitPageInto3(tmpKeys, tmpSubPages, pChild1, pChild2, pChild3, oi1, oi2);

       // copy the first element to the root
       m_Keys    [pos] = oi1;
       m_SubPages[pos] = pChild1;

       // copy the second element to the root
       ::insert_at(m_Keys, oi2, pos+1);
       ::insert_at(m_SubPages, pChild2, pos+1);
       NumberOfKeys()++;

       m_SubPages[pos+2] = pChild3;
}

template <typename Traits>
void CBTreePage<Traits>::SplitPageInto3(vector<ObjectInfo>& tmpKeys,
                                                vector<BTPage *>  & tmpSubPages,
                                                BTPage*                   &     pChild1,
                                                BTPage*                   &     pChild2,
                                                BTPage*                   &     pChild3,
                                                ObjectInfo                & oi1,
                                                ObjectInfo                & oi2)
{
       assert(tmpKeys.size() >= 8);
       assert(tmpSubPages.size() >= 9);
       if( !pChild1 )
               pChild1 = new BTPage(m_MaxKeysForChilds, m_Unique);

       // Split tmpKeys page into 3 pages
       // copy 1/3 elements to the first child
       pChild1->clear();
       int nKeys = (tmpKeys.size()-2)/3;
       int i = 0;
       for( ; i < nKeys; i++ )
       {
               pChild1->m_Keys    [i] = tmpKeys    [i];
               pChild1->m_SubPages[i] = tmpSubPages[i];
               pChild1->NumberOfKeys()++;
       }
       pChild1->m_SubPages[i] = tmpSubPages[i];

       // first element to go up !
       oi1 = tmpKeys[i++];

       if( !pChild2 )
               pChild2 = new BTPage(m_MaxKeysForChilds, m_Unique);
       pChild2->clear();
       // copy 1/3 to the second child
       nKeys += (tmpKeys.size()-2)/3 + 1;
       int j = 0;
       for(; i < nKeys; i++, j++ )
       {
               pChild2->m_Keys    [j] = tmpKeys    [i];
               pChild2->m_SubPages[j] = tmpSubPages[i];
               pChild2->NumberOfKeys()++;
       }
       pChild2->m_SubPages[j] = tmpSubPages[i];

       // copy the second element to the root
       oi2 = tmpKeys[i++];

       // copy 1/3 to the third child
       if( !pChild3 )
               pChild3 = new BTPage(m_MaxKeysForChilds, m_Unique);
       pChild3->clear();
       nKeys = tmpKeys.size();
       for(j = 0; i < nKeys; i++, j++)
       {
               pChild3->m_Keys    [j] = tmpKeys    [i];
               pChild3->m_SubPages[j] = tmpSubPages[i];
               pChild3->NumberOfKeys()++;
       }
       pChild3->m_SubPages[j] = tmpSubPages[i];
}

template <typename Traits>
bool CBTreePage<Traits>::SplitRoot()
{
       BTPage  *pChild1 = 0, *pChild2 = 0, *pChild3 = 0;
       ObjectInfo oi1, oi2;
       SplitPageInto3( m_Keys,m_SubPages,pChild1, pChild2, pChild3, oi1, oi2);
       clear();

       // copy the first element to the root
       m_Keys    [0] = oi1;
       m_SubPages[0] = pChild1;
       NumberOfKeys()++;

       // copy the second element to the root
       m_Keys    [1] = oi2;
       m_SubPages[1] = pChild2;
       NumberOfKeys()++;

       m_SubPages[2] = pChild3;
       return true;
}

template <typename Traits>
typename CBTreePage<Traits>::ObjectInfo*
CBTreePage<Traits>::Search(const value_type& key)
{
        int pos = binary_search(m_Keys, 0, m_KeyCount, key);
        if( pos >= m_KeyCount ) {
                if( m_SubPages[pos] )
                        return m_SubPages[pos]->Search(key);
                return nullptr;
        }
        if( key == m_Keys[pos].key ) {
                m_Keys[pos].UseCounter++;
                return &m_Keys[pos];
        }
        if( key < m_Keys[pos].key )
                if( m_SubPages[pos] )
                        return m_SubPages[pos]->Search(key);
        return nullptr;
}

template <typename Traits>
template <typename Func, typename... Args>
void CBTreePage<Traits>::Inorden(Func fn, int level, Args&&... args)
{
        for( int i = 0 ; i < m_KeyCount ; i++ ) {
                if( m_SubPages[i] )
                        m_SubPages[i]->Inorden(fn, level+1, std::forward<Args>(args)...);
                fn(m_Keys[i], level, args...);
        }
        if( m_SubPages[m_KeyCount] )
                m_SubPages[m_KeyCount]->Inorden(fn, level+1, std::forward<Args>(args)...);
}

template <typename Traits>
template <typename Func, typename... Args>
void CBTreePage<Traits>::Preorden(Func fn, int level, Args&&... args)
{
        for( int i = 0 ; i < m_KeyCount ; i++ ) {
                fn(m_Keys[i], level, args...);
                if( m_SubPages[i] )
                        m_SubPages[i]->Preorden(fn, level+1, std::forward<Args>(args)...);
        }
        if( m_SubPages[m_KeyCount] )
                m_SubPages[m_KeyCount]->Preorden(fn, level+1, std::forward<Args>(args)...);
}

template <typename Traits>
template <typename Func, typename... Args>
void CBTreePage<Traits>::Postorden(Func fn, int level, Args&&... args)
{
        for( int i = 0 ; i < m_KeyCount ; i++ )
                if( m_SubPages[i] )
                        m_SubPages[i]->Postorden(fn, level+1, std::forward<Args>(args)...);
        if( m_SubPages[m_KeyCount] )
                m_SubPages[m_KeyCount]->Postorden(fn, level+1, std::forward<Args>(args)...);
        for( int i = 0 ; i < m_KeyCount ; i++ )
                fn(m_Keys[i], level, args...);
}

template <typename Traits>
template <typename Func, typename... Args>
void CBTreePage<Traits>::Foreach(Func fn, int level, Args&&... args)
{
        Inorden(fn, level, std::forward<Args>(args)...);
}

template <typename Traits>
template <typename Func, typename... Args>
typename CBTreePage<Traits>::ObjectInfo*
CBTreePage<Traits>::FirstThat(Func fn, int level, Args&&... args)
{
        ObjectInfo* pFound = nullptr;
        for( int i = 0 ; i < m_KeyCount ; i++ ) {
                if( m_SubPages[i] ) {
                        pFound = m_SubPages[i]->FirstThat(fn, level+1, std::forward<Args>(args)...);
                        if( pFound ) return pFound;
                }
                if( fn(m_Keys[i], level, args...) ) return &m_Keys[i];
        }
        if( m_SubPages[m_KeyCount] ) {
                pFound = m_SubPages[m_KeyCount]->FirstThat(fn, level+1, std::forward<Args>(args)...);
                if( pFound ) return pFound;
        }
        return nullptr;
}

template <typename Traits>
bt_ErrorCode CBTreePage<Traits>::Remove(const value_type& key, ObjIDType ObjID)
{
        bt_ErrorCode error = bt_ok;
        int pos = binary_search(m_Keys, 0, m_KeyCount, key);
       if( pos < NumberOfKeys() && key == m_Keys[pos].key /*&& m_Keys[pos].m_ObjID == ObjID*/) // We found it !
        {
               // This is a leave: First
               if( !m_SubPages[pos+1] )  // This is a leave ? FIRST CASE !
                {
                        ::remove(m_Keys, pos);
                        NumberOfKeys()--;
                       if( Underflow() )
                               return bt_underflow;
                        return bt_ok;
                }

               // We FOUND IT BUT it is NOT a leave ? SECOND CASE !
                {
                       // Get the first element from right branch
                       ObjectInfo &rFirstFromRight = m_SubPages[pos+1]->GetFirstObjectInfo();
                       // change with a leave
                        swap(m_Keys[pos], rFirstFromRight);
                        error = m_SubPages[++pos]->Remove(key, ObjID);
                }
        }
       else if( pos == NumberOfKeys() ) // it is not here, go by the last branch
                error = m_SubPages[pos]->Remove(key, ObjID);
       else if( key <= m_Keys[pos].key ){ // = is because identical keys are inserted on left (see Insert)
                if( m_SubPages[pos] )
                        error = m_SubPages[pos]->Remove(key, ObjID);
                else
                        return bt_nofound;
        }
       if( error == bt_underflow ){
               // THIRD CASE: After removing the element we have an underflow
               //Print(cout);
               if( TreatUnderflow(pos) )
                       return bt_ok;
               // FOURTH CASE: it was not possible to redistribute -> Merge
               if( IsRoot() && NumberOfKeys() == 2 )
                       return MergeRoot();
                return Merge(pos);
        }
       if( error == bt_nofound )
               return bt_nofound;
        return bt_ok;
}


template <typename Traits>
bt_ErrorCode CBTreePage<Traits>::Merge(int pos)
{
       assert( m_SubPages[pos-1]->NumberOfKeys() +
                       m_SubPages[ pos ]->NumberOfKeys() +
                       m_SubPages[pos+1]->NumberOfKeys() ==
                       3*m_SubPages[ pos ]->MinNumberOfKeys() - 1);

       // FIRST: Put all the elements into a vector
       vector<ObjectInfo> tmpKeys;
       //tmpKeys.resize(nKeys);
       vector<BTPage *>   tmpSubPages;

       BTPage  *pChild1 = m_SubPages[pos-1],
                       *pChild2 = m_SubPages[ pos ],
                       *pChild3 = m_SubPages[pos+1];
       MovePage(pChild1, tmpKeys, tmpSubPages);
       tmpKeys    .push_back(m_Keys[pos-1]);
       MovePage(pChild2, tmpKeys, tmpSubPages);
       tmpKeys    .push_back(m_Keys[ pos ]);
       MovePage(pChild3, tmpKeys, tmpSubPages);
       pChild3->Destroy();;

       // Move 1/2 elements to pChild1
       int nKeys = pChild1->GetFreeCells();
       int i = 0;
       for( ; i < nKeys ; i++ )
       {
               pChild1->m_Keys    [i] = tmpKeys    [i];
               pChild1->m_SubPages[i] = tmpSubPages[i];
               pChild1->NumberOfKeys()++;
       }
       pChild1->m_SubPages[i] = tmpSubPages[i];

       m_Keys    [pos-1] = tmpKeys[i];
       m_SubPages[pos-1] = pChild1;

       ::remove(m_Keys    , pos);
       ::remove(m_SubPages, pos);
       NumberOfKeys()--;

       nKeys = pChild2->GetFreeCells();
       int j = ++i;
       for(i = 0 ; i < nKeys ; i++, j++ )
       {
               pChild2->m_Keys    [i] = tmpKeys    [j];
               pChild2->m_SubPages[i] = tmpSubPages[j];
               pChild2->NumberOfKeys()++;
       }
       pChild2->m_SubPages[i] = tmpSubPages[j];
       m_SubPages[ pos ]          = pChild2;

       if( Underflow() )
               return bt_underflow;
       return bt_ok;
}

template <typename Traits>
bt_ErrorCode CBTreePage<Traits>::MergeRoot()
{
       int pos = 1;
       assert( m_SubPages[pos-1]->NumberOfKeys() +
                       m_SubPages[ pos ]->NumberOfKeys() +
                       m_SubPages[pos+1]->NumberOfKeys() ==
                       3*m_SubPages[ pos ]->MinNumberOfKeys() - 1);

       BTPage  *pChild1 = m_SubPages[pos-1], *pChild2 = m_SubPages[ pos ], *pChild3 = m_SubPages[pos+1];
       int nKeys = pChild1->NumberOfKeys() + pChild2->NumberOfKeys() + pChild3->NumberOfKeys() + 2;

       // FIRST: Put all the elements into a vector
       vector<ObjectInfo> tmpKeys;
       //tmpKeys.resize(nKeys);
       vector<BTPage *>   tmpSubPages;

       MovePage(pChild1, tmpKeys, tmpSubPages);
       tmpKeys    .push_back(m_Keys[pos-1]);
       MovePage(pChild2, tmpKeys, tmpSubPages);
       tmpKeys    .push_back(m_Keys[ pos ]);
       MovePage(pChild3, tmpKeys, tmpSubPages);

       clear();
       int i = 0;
       for( ; i < nKeys ; i++ ){
               m_Keys    [i] = tmpKeys    [i];
               m_SubPages[i] = tmpSubPages[i];
               NumberOfKeys()++;
       }
       m_SubPages[i] = tmpSubPages[i];

       //Print(cout);
       pChild1->Destroy();
       pChild2->Destroy();
       pChild3->Destroy();

       return bt_rootmerged;
}

template <typename Traits>
typename CBTreePage<Traits>::ObjectInfo &
CBTreePage<Traits>::GetFirstObjectInfo()
{
       if( m_SubPages[0] )
               return m_SubPages[0]->GetFirstObjectInfo();
       return m_Keys[0];
}

template <typename Traits>
void CBTreePage<Traits>::Print(ostream& os)
{
        Inorden([](CBTreeEntry<Traits>& info, int level, ostream* pOs) {
                for( int i = 0; i < level ; i++) *pOs << "\t";
                *pOs << info.key << "->" << info.ObjID << "\n";
        }, 0, &os);
}

template <typename Traits>
void CBTreePage<Traits>::Create()
{
       Reset();
       m_Keys.resize(m_MaxKeys+1);
       m_SubPages.resize(m_MaxKeys+2, NULL);
       m_KeyCount = 0;
       m_MinKeys  = 2 * m_MaxKeys/3;
}

template <typename Traits>
void CBTreePage<Traits>::Reset()
{
       for( int i = 0 ; i < m_KeyCount ; i++ )
               delete m_SubPages[i];
       clear();
}

template <typename Traits>
void CBTreePage<Traits>::clear()
{
       //m_Keys.clear();
       //m_SubPages.clear();
       m_KeyCount = 0;
}

template <typename Traits>
CBTreePage<Traits>* CreateBTreeNode (int maxKeys, int unique)
{
       return new CBTreePage<Traits> (maxKeys, unique);
}

template <typename Traits>
void CBTreePage<Traits>::MovePage(BTPage *pChildPage, vector<ObjectInfo> &tmpKeys,vector<BTPage *> &tmpSubPages)
{
       int nKeys = pChildPage->GetNumberOfKeys();
       int i = 0;
       for( ; i < nKeys; i++ )
       {
               tmpKeys    .push_back(pChildPage->m_Keys[i]);
               tmpSubPages.push_back(pChildPage->m_SubPages[i]);
       }
       tmpSubPages.push_back(pChildPage->m_SubPages[i]);
       pChildPage->clear();
}

template <typename Traits>
int CBTreePage<Traits>::GetFreeCellsOnLeft(int pos)
{
       if( pos > 0 )                                   // there is some page on left ?
               return m_SubPages[pos-1]->GetFreeCells();
       return 0;
}

template <typename Traits>
int CBTreePage<Traits>::GetFreeCellsOnRight(int pos)
{
       if( pos < GetNumberOfKeys() )   // there is some page on right ?
               return m_SubPages[pos+1]->GetFreeCells();
       return 0;
}

#endif