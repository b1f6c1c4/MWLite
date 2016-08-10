#pragma once
#include "../stdafx.h"
#include "DenseSet.h"
#include "SparseSet.h"
#include <bitset>

#define BIN_T(x) (static_cast<bin_t>(x))
#define CEIL_DIV(x, y) ((x) + (y) - BIN_T(1)) / (y)
#define BIN_SIZE BIN_T(sizeof(bin_t) * CHAR_BIT)
#define BIN_NUM(x) CEIL_DIV((x), BIN_SIZE)
#define BIN_ID(x) ((x) / BIN_SIZE)
#define BIT_ID(x) ((x) % BIN_SIZE)
#define GET_BIT(x, n) (((x) >> (n)) & BIN_T(1))
#define SET_BIT(x, n) (x) |= (BIN_T(1) << (n))
#define UNSET_BIT(x, n) (x) &= ~(BIN_T(1) << (n))
#define COUNT_ONE(x) (std::bitset<sizeof(x) * CHAR_BIT>(x).count())

template <typename T>
class DenseSet;

template <typename T>
class SparseSet;

template <typename T>
class DenseBitSet sealed
{
    typedef size_t bin_t;

public:
    explicit DenseBitSet(size_t full);
    template <template <typename T> typename Set>
    explicit DenseBitSet(const Set<T> &other);

    DEFAULT_COPY(DenseBitSet<T>);
    DEFAULT_MOVE(DenseBitSet<T>);

    bool operator[](T id) const;

    size_t Count() const;

    size_t FullSize() const;

    void Clear();

    DenseBitSet<T> &operator+=(T id);

    DenseBitSet<T> &operator-=(T id);

    DenseBitSet<T> &operator+=(const DenseBitSet<T> &other);

    DenseBitSet<T> &operator-=(const DenseBitSet<T> &other);

    DenseBitSet<T> &operator*=(const DenseBitSet<T> &other);

    DenseBitSet<T> operator!();

    class const_iterator
    {
    public:
        typedef std::forward_iterator_tag iterator_category;

        const_iterator(const bin_t *ptr, size_t id, bin_t bitId, size_t full);

        const_iterator operator++(int);

        const_iterator operator++();

        T operator*() const;

        bool operator==(const const_iterator &rhs) const;

        bool operator!=(const const_iterator &rhs) const;

    private:
        const bin_t *m_Ptr;
        size_t m_Id;
        size_t m_BitId;
        size_t m_FullSize;
    };

    const_iterator begin() const;
    const_iterator end() const;

private:
    size_t m_FullSize;

    std::vector<bin_t> m_DenseBitSet;
};

template <typename T>
DenseBitSet<T>::DenseBitSet(size_t full) : m_FullSize(full), m_DenseBitSet(BIN_NUM(full), BIN_T(0)) { }

template <typename T>
template <template <typename T> class Set>
DenseBitSet<T>::DenseBitSet(const Set<T> &other) : m_FullSize(other.FullSize()), m_DenseBitSet(BIN_NUM(other.FullSize()), BIN_T(0))
{
    for (auto &id : other)
        (*this) += id;
}

template <typename T>
bool DenseBitSet<T>::operator[](T id) const
{
    return GET_BIT(m_DenseBitSet[BIN_ID(id)], BIT_ID(id));
}

template <typename T>
size_t DenseBitSet<T>::Count() const
{
    size_t c = 0;
    for (size_t i = 0; i < m_DenseBitSet.size(); i++)
        if (i == m_DenseBitSet.size() - 1)
            c += COUNT_ONE(m_DenseBitSet[i] & (BIN_T(1) << BIT_ID(FullSize())) - BIN_T(1));
        else
            c += COUNT_ONE(m_DenseBitSet[i]);

    //    for (auto t = v; t != BIN_T(0); t >>= BIN_T(1))
    //        if (t & BIN_T(1))
    //            ++c;

    return c;
}

template <typename T>
size_t DenseBitSet<T>::FullSize() const
{
    return m_FullSize;
}

template <typename T>
void DenseBitSet<T>::Clear()
{
    memset(&*m_DenseBitSet.begin(), BIN_T(0), sizeof(bin_t) * m_DenseBitSet.size());
}

template <typename T>
DenseBitSet<T> &DenseBitSet<T>::operator+=(T id)
{
    ASSERT(0 <= id && id < FullSize());
    SET_BIT(m_DenseBitSet[BIN_ID(id)], BIT_ID(id));
    return *this;
}

template <typename T>
DenseBitSet<T> &DenseBitSet<T>::operator-=(T id)
{
    ASSERT(0 <= id && id < FullSize());
    UNSET_BIT(m_DenseBitSet[BIN_ID(id)], BIT_ID(id));
    return *this;
}

template <typename T>
DenseBitSet<T> &DenseBitSet<T>::operator+=(const DenseBitSet<T> &other)
{
    ASSERT(FullSize() == other.FullSize());

    for (size_t i = 0; i < m_DenseBitSet.size(); i++)
        m_DenseBitSet[i] |= other.m_DenseBitSet[i];
    return *this;
}

template <typename T>
DenseBitSet<T> &DenseBitSet<T>::operator-=(const DenseBitSet<T> &other)
{
    ASSERT(FullSize() == other.FullSize());

    for (size_t i = 0; i < m_DenseBitSet.size(); i++)
        m_DenseBitSet[i] &= ~other.m_DenseBitSet[i];
    return *this;
}

template <typename T>
DenseBitSet<T> &DenseBitSet<T>::operator*=(const DenseBitSet<T> &other)
{
    ASSERT(FullSize() == other.FullSize());

    for (size_t i = 0; i < m_DenseBitSet.size(); i++)
        m_DenseBitSet[i] &= other.m_DenseBitSet[i];
    return *this;
}

template <typename T>
DenseBitSet<T> DenseBitSet<T>::operator!()
{
    DenseBitSet<T> set(0);
    set.m_FullSize = m_FullSize;

    for (size_t i = 0; i < m_DenseBitSet.size(); i++)
        set.m_DenseBitSet.push_back(~m_DenseBitSet[i]);
    return set;
}

template <typename T>
DenseBitSet<T>::const_iterator::const_iterator(const bin_t *ptr, size_t id, bin_t bitId, size_t full) : m_Ptr(ptr), m_Id(id), m_BitId(bitId), m_FullSize(full) { }

template <typename T>
typename DenseBitSet<T>::const_iterator DenseBitSet<T>::const_iterator::operator++(int)
{
    auto i = *this;
    ++(*this);
    return i;
}

template <typename T>
typename DenseBitSet<T>::const_iterator DenseBitSet<T>::const_iterator::operator++()
{
    while (m_Id != m_FullSize)
    {
        ++m_Id;
        if (++m_BitId >= BIN_SIZE)
        {
            m_BitId = BIN_T(0);
            ++m_Ptr;
        }
        if (m_Id == m_FullSize)
            break;
        if (GET_BIT(*m_Ptr, m_BitId))
            return *this;
    }
    return *this;
}

template <typename T>
T DenseBitSet<T>::const_iterator::operator*() const
{
    return m_Id;
}

template <typename T>
bool DenseBitSet<T>::const_iterator::operator==(const const_iterator &rhs) const
{
    return m_Id == rhs.m_Id;
}

template <typename T>
bool DenseBitSet<T>::const_iterator::operator!=(const const_iterator &rhs) const
{
    return !(*this == rhs);
}

template <typename T>
typename DenseBitSet<T>::const_iterator DenseBitSet<T>::begin() const
{
    return ++const_iterator(&*m_DenseBitSet.begin() - 1, BIN_T(-1), BIN_SIZE, FullSize());
}

template <typename T>
typename DenseBitSet<T>::const_iterator DenseBitSet<T>::end() const
{
    return const_iterator(&*m_DenseBitSet.begin(), FullSize(), BIN_T(0), FullSize());
}

template <typename T>
DenseBitSet<T> operator+(DenseBitSet<T> lhs, const DenseBitSet<T> &rhs)
{
    return lhs += rhs;
}

template <typename T>
DenseBitSet<T> operator-(DenseBitSet<T> lhs, const DenseBitSet<T> &rhs)
{
    return lhs -= rhs;
}

template <typename T>
DenseBitSet<T> operator*(DenseBitSet<T> lhs, const DenseBitSet<T> &rhs)
{
    return lhs *= rhs;
}
