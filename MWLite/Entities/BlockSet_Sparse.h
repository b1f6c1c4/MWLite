#pragma once
#include "../stdafx.h"
#include <algorithm>

template <typename T>
class Set sealed
{
public:
    explicit Set(size_t full);

    DEFAULT_COPY(Set<T>);
    DEFAULT_MOVE(Set<T>);

    bool operator[](T id) const;

    size_t Count() const;

    size_t FullSize() const;

    void Clear();

    Set<T> &operator+=(T id);

    Set<T> &operator-=(T id);

    Set<T> &operator+=(const Set<T> &other);

    Set<T> &operator-=(const Set<T> &other);

    Set<T> &operator*=(const Set<T> &other);

    Set<T> operator!();

    typename std::vector<T>::const_iterator begin() const;

    typename std::vector<T>::const_iterator end() const;

private:
    size_t m_FullSize;
    std::vector<T> m_Set;
};

template <typename T>
Set<T>::Set(size_t full) : m_FullSize(full) { }

template <typename T>
bool Set<T>::operator[](T id) const
{
    auto lb = std::lower_bound(m_Set.begin, m_Set.end(), id);
    if (lb == m_Set.end())
        return false;
    return *lb == id;
}

template <typename T>
size_t Set<T>::Count() const
{
    return m_Set.size();
}

template <typename T>
size_t Set<T>::FullSize() const
{
    return m_FullSize;
}

template <typename T>
void Set<T>::Clear()
{
    m_Set.clear();
}

template <typename T>
Set<T> &Set<T>::operator+=(T id)
{
    auto lb = std::lower_bound(m_Set.begin, m_Set.end(), id);
    if (lb != m_Set.end() && *lb == id)
        return *this;
    m_Set.insert(lb, id);
    return *this;
}

template <typename T>
Set<T> &Set<T>::operator-=(T id)
{
    auto lb = std::lower_bound(m_Set.begin, m_Set.end(), id);
    if (lb == m_Set.end() || *lb != id)
        return *this;
    m_Set.erase(lb);
    return *this;
}

template <typename T>
Set<T> &Set<T>::operator+=(const Set<T> &other)
{
    ASSERT(FullSize() == other.FullSize());

    for (auto id : other.m_Set)
        *this += id;
    return *this;
}

template <typename T>
Set<T> &Set<T>::operator-=(const Set<T> &other)
{
    ASSERT(FullSize() == other.FullSize());

    for (auto id : other.m_Set)
        *this -= id;
    return *this;
}

template <typename T>
Set<T> &Set<T>::operator*=(const Set<T> &other)
{
    ASSERT(FullSize() == other.FullSize());

    for (size_t i = 0; i < m_Set.size(); i++)
        if (!other[m_Set[i]])
            m_Set.erase(m_Set.begin() + i);
    return *this;
}

template <typename T>
Set<T> Set<T>::operator!()
{
    Set<T> set(FullSize());

    for (size_t i = 0; i < FullSize(); i++)
        if (!(*this)[i])
            set += i;
    return set;
}

template <typename T>
typename std::vector<T>::const_iterator Set<T>::begin() const
{
    return m_Set.begin();
}

template <typename T>
typename std::vector<T>::const_iterator Set<T>::end() const
{
    return m_Set.end();
}
