#pragma once
#include "../stdafx.h"

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
    std::vector<int_fast8_t> m_Set;
};

template <typename T>
Set<T>::Set(size_t full) : m_Set(full, false) { }

template <typename T>
bool Set<T>::operator[](T id) const
{
    return m_Set[id];
}

template <typename T>
size_t Set<T>::Count() const
{
    size_t c = 0;
    for (auto &v : m_Set)
        if (v)
            c++;
    return c;
}

template <typename T>
size_t Set<T>::FullSize() const
{
    return m_Set.size();
}

template <typename T>
void Set<T>::Clear()
{
    for (auto &v : m_Set)
        v = false;
}

template <typename T>
Set<T> &Set<T>::operator+=(T id)
{
    m_Set[id] = true;
    return *this;
}

template <typename T>
Set<T> &Set<T>::operator-=(T id)
{
    m_Set[id] = false;
    return *this;
}

template <typename T>
Set<T> &Set<T>::operator+=(const Set<T> &other)
{
    ASSERT(FullSize() == other.FullSize());

    for (size_t i = 0; i < FullSize(); i++)
        m_Set[i] |= other[i];
    return *this;
}

template <typename T>
Set<T> &Set<T>::operator-=(const Set<T> &other)
{
    ASSERT(FullSize() == other.FullSize());

    for (size_t i = 0; i < FullSize(); i++)
        m_Set[i] &= !other[i];
    return *this;
}

template <typename T>
Set<T> &Set<T>::operator*=(const Set<T> &other)
{
    ASSERT(FullSize() == other.FullSize());

    for (size_t i = 0; i < FullSize(); i++)
        m_Set[i] &= other[i];
    return *this;
}

template <typename T>
Set<T> Set<T>::operator!()
{
    Set<T> set(FullSize());

    for (size_t i = 0; i < FullSize(); i++)
        set[i] = !m_Set[i];
    return set;
}

template <typename T>
Set<T>::const_iterator::const_iterator(const int_fast8_t *begin, const int_fast8_t *ptr, const int_fast8_t *end) : m_Begin(begin), m_Ptr(ptr), m_End(end) { }

template <typename T>
typename Set<T>::const_iterator Set<T>::const_iterator::operator++(int)
{
    auto i = *this;
    ++(*this);
    return i;
}

template <typename T>
typename Set<T>::const_iterator Set<T>::const_iterator::operator++()
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
T Set<T>::const_iterator::operator*() const
{
    ASSERT(*m_Ptr);
    return m_Ptr - m_Begin;
}

template <typename T>
bool Set<T>::const_iterator::operator==(const const_iterator &rhs) const
{
    return m_Ptr == rhs.m_Ptr;
}

template <typename T>
bool Set<T>::const_iterator::operator!=(const const_iterator &rhs) const
{
    return !(*this == rhs);
}

template <typename T>
typename Set<T>::const_iterator Set<T>::begin() const
{
    return ++const_iterator(m_Set.begin(), m_Set.begin() - 1, m_Set.end());
}

template <typename T>
typename Set<T>::const_iterator Set<T>::end() const
{
    return const_iterator(m_Set.begin(), m_Set.end(), m_Set.end());
}
