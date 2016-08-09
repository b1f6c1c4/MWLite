#pragma once
#include "../stdafx.h"
#include "DenseSet.h"
#include <algorithm>

template <typename T>
class DenseSet;

template <typename T>
class SparseSet sealed
{
public:
    explicit SparseSet(size_t full);
    // ReSharper disable once CppNonExplicitConvertingConstructor
    SparseSet(const DenseSet<T> &other);

    DEFAULT_COPY(SparseSet<T>);
    DEFAULT_MOVE(SparseSet<T>);

    bool operator[](T id) const;

    size_t Count() const;

    size_t FullSize() const;

    void Clear();

    SparseSet<T> &operator+=(T id);

    SparseSet<T> &operator-=(T id);

    SparseSet<T> &operator+=(const SparseSet<T> &other);

    SparseSet<T> &operator-=(const SparseSet<T> &other);

    SparseSet<T> &operator*=(const SparseSet<T> &other);

    SparseSet<T> operator!();

    typename std::vector<T>::const_iterator begin() const;

    typename std::vector<T>::const_iterator end() const;

private:
    size_t m_FullSize;
    std::vector<T> m_SparseSet;
};

template <typename T>
SparseSet<T>::SparseSet(size_t full) : m_FullSize(full) { }

template <typename T>
SparseSet<T>::SparseSet(const DenseSet<T> &other)
{
    for (auto &id : other)
        (*this) += id;
}

template <typename T>
bool SparseSet<T>::operator[](T id) const
{
    auto lb = std::lower_bound(m_SparseSet.begin(), m_SparseSet.end(), id);
    if (lb == m_SparseSet.end())
        return false;
    return *lb == id;
}

template <typename T>
size_t SparseSet<T>::Count() const
{
    return m_SparseSet.size();
}

template <typename T>
size_t SparseSet<T>::FullSize() const
{
    return m_FullSize;
}

template <typename T>
void SparseSet<T>::Clear()
{
    m_SparseSet.clear();
}

template <typename T>
SparseSet<T> &SparseSet<T>::operator+=(T id)
{
    auto lb = std::lower_bound(m_SparseSet.begin(), m_SparseSet.end(), id);
    if (lb != m_SparseSet.end() && *lb == id)
        return *this;
    m_SparseSet.insert(lb, id);
    return *this;
}

template <typename T>
SparseSet<T> &SparseSet<T>::operator-=(T id)
{
    auto lb = std::lower_bound(m_SparseSet.begin(), m_SparseSet.end(), id);
    if (lb == m_SparseSet.end() || *lb != id)
        return *this;
    m_SparseSet.erase(lb);
    return *this;
}

template <typename T>
SparseSet<T> &SparseSet<T>::operator+=(const SparseSet<T> &other)
{
    ASSERT(FullSize() == other.FullSize());

    for (auto id : other.m_SparseSet)
        *this += id;
    return *this;
}

template <typename T>
SparseSet<T> &SparseSet<T>::operator-=(const SparseSet<T> &other)
{
    ASSERT(FullSize() == other.FullSize());

    for (auto id : other.m_SparseSet)
        *this -= id;
    return *this;
}

template <typename T>
SparseSet<T> &SparseSet<T>::operator*=(const SparseSet<T> &other)
{
    ASSERT(FullSize() == other.FullSize());

    for (size_t i = 0; i < m_SparseSet.size(); i++)
        if (!other[m_SparseSet[i]])
            m_SparseSet.erase(m_SparseSet.begin() + i);
    return *this;
}

template <typename T>
SparseSet<T> SparseSet<T>::operator!()
{
    SparseSet<T> SparseSet(FullSize());

    for (size_t i = 0; i < FullSize(); i++)
        if (!(*this)[i])
            SparseSet += i;
    return SparseSet;
}

template <typename T>
typename std::vector<T>::const_iterator SparseSet<T>::begin() const
{
    return m_SparseSet.begin();
}

template <typename T>
typename std::vector<T>::const_iterator SparseSet<T>::end() const
{
    return m_SparseSet.end();
}

template <typename T>
SparseSet<T> operator+(SparseSet<T> lhs, const SparseSet<T> &rhs)
{
    return lhs += rhs;
}

template <typename T>
SparseSet<T> operator-(SparseSet<T> lhs, const SparseSet<T> &rhs)
{
    return lhs -= rhs;
}

template <typename T>
SparseSet<T> operator*(SparseSet<T> lhs, const SparseSet<T> &rhs)
{
    return lhs *= rhs;
}
