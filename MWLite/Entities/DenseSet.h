#pragma once
#include "../stdafx.h"
#include "SparseSet.h"

template <typename T>
class SparseSet;

template <typename T>
class DenseSet sealed
{
public:
    explicit DenseSet(size_t full);
    template <template <typename T> typename Set>
    explicit DenseSet(const Set<T> &other);

    DEFAULT_COPY(DenseSet<T>);
    DEFAULT_MOVE(DenseSet<T>);

    bool operator[](T id) const;

    size_t Count() const;

    size_t FullSize() const;

    void Clear();

    DenseSet<T> &operator+=(T id);

    DenseSet<T> &operator-=(T id);

    DenseSet<T> &operator+=(const DenseSet<T> &other);

    DenseSet<T> &operator-=(const DenseSet<T> &other);

    DenseSet<T> &operator*=(const DenseSet<T> &other);

    DenseSet<T> operator!();

    class const_iterator
    {
    public:
        typedef std::forward_iterator_tag iterator_category;

        const_iterator(const int_fast8_t *begin, const int_fast8_t *ptr, const int_fast8_t *end);

        const_iterator operator++(int);

        const_iterator operator++();

        T operator*() const;

        bool operator==(const const_iterator &rhs) const;

        bool operator!=(const const_iterator &rhs) const;

    private:
        const int_fast8_t *m_Begin;
        const int_fast8_t *m_Ptr;
        const int_fast8_t *m_End;
    };

    const_iterator begin() const;
    const_iterator end() const;

private:
    std::vector<int_fast8_t> m_DenseSet;
};

template <typename T>
DenseSet<T>::DenseSet(size_t full) : m_DenseSet(full, false) { }

template <typename T>
template <template <typename T> class Set>
DenseSet<T>::DenseSet(const Set<T> &other)
{
    for (auto &id : other)
        (*this) += id;
}

template <typename T>
bool DenseSet<T>::operator[](T id) const
{
    return m_DenseSet[id];
}

template <typename T>
size_t DenseSet<T>::Count() const
{
    size_t c = 0;
    for (auto &v : m_DenseSet)
        if (v)
            c++;
    return c;
}

template <typename T>
size_t DenseSet<T>::FullSize() const
{
    return m_DenseSet.size();
}

template <typename T>
void DenseSet<T>::Clear()
{
    for (auto &v : m_DenseSet)
        v = false;
}

template <typename T>
DenseSet<T> &DenseSet<T>::operator+=(T id)
{
    m_DenseSet[id] = true;
    return *this;
}

template <typename T>
DenseSet<T> &DenseSet<T>::operator-=(T id)
{
    m_DenseSet[id] = false;
    return *this;
}

template <typename T>
DenseSet<T> &DenseSet<T>::operator+=(const DenseSet<T> &other)
{
    ASSERT(FullSize() == other.FullSize());

    for (size_t i = 0; i < FullSize(); i++)
        m_DenseSet[i] |= other[i];
    return *this;
}

template <typename T>
DenseSet<T> &DenseSet<T>::operator-=(const DenseSet<T> &other)
{
    ASSERT(FullSize() == other.FullSize());

    for (size_t i = 0; i < FullSize(); i++)
        m_DenseSet[i] &= !other[i];
    return *this;
}

template <typename T>
DenseSet<T> &DenseSet<T>::operator*=(const DenseSet<T> &other)
{
    ASSERT(FullSize() == other.FullSize());

    for (size_t i = 0; i < FullSize(); i++)
        m_DenseSet[i] &= other[i];
    return *this;
}

template <typename T>
DenseSet<T> DenseSet<T>::operator!()
{
    DenseSet<T> DenseSet(FullSize());

    for (size_t i = 0; i < FullSize(); i++)
        if (!m_DenseSet[i])
            DenseSet += i;
    return DenseSet;
}

template <typename T>
DenseSet<T>::const_iterator::const_iterator(const int_fast8_t *begin, const int_fast8_t *ptr, const int_fast8_t *end) : m_Begin(begin), m_Ptr(ptr), m_End(end) { }

template <typename T>
typename DenseSet<T>::const_iterator DenseSet<T>::const_iterator::operator++(int)
{
    auto i = *this;
    ++(*this);
    return i;
}

template <typename T>
typename DenseSet<T>::const_iterator DenseSet<T>::const_iterator::operator++()
{
    while (m_Ptr != m_End)
    {
        ++m_Ptr;
        if (*m_Ptr)
            return *this;
    }
    return *this;
}

template <typename T>
T DenseSet<T>::const_iterator::operator*() const
{
    ASSERT(*m_Ptr);
    return m_Ptr - m_Begin;
}

template <typename T>
bool DenseSet<T>::const_iterator::operator==(const const_iterator &rhs) const
{
    return m_Ptr == rhs.m_Ptr;
}

template <typename T>
bool DenseSet<T>::const_iterator::operator!=(const const_iterator &rhs) const
{
    return !(*this == rhs);
}

template <typename T>
typename DenseSet<T>::const_iterator DenseSet<T>::begin() const
{
    return ++const_iterator(&*m_DenseSet.begin(), &*m_DenseSet.begin() - 1, &*(m_DenseSet.end() - 1) + 1);
}

template <typename T>
typename DenseSet<T>::const_iterator DenseSet<T>::end() const
{
    return const_iterator(&*m_DenseSet.begin(), &*(m_DenseSet.end() - 1) + 1, &*(m_DenseSet.end() - 1) + 1);
}

template <typename T>
DenseSet<T> operator+(DenseSet<T> lhs, const DenseSet<T> &rhs)
{
    return lhs += rhs;
}

template <typename T>
DenseSet<T> operator-(DenseSet<T> lhs, const DenseSet<T> &rhs)
{
    return lhs -= rhs;
}

template <typename T>
DenseSet<T> operator*(DenseSet<T> lhs, const DenseSet<T> &rhs)
{
    return lhs *= rhs;
}
