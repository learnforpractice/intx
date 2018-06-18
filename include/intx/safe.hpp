// Copyright 2017 Pawel Bylica.
// Licensed under the Apache License, Version 2.0. See the LICENSE file.

#pragma once

#include <limits>
#include <stdexcept>

namespace intx
{
enum class status : char
{
    normal = 0,
    plus_infinity,
    minus_infinity,
    invalid
};

class bad_operation : public std::exception
{};

template<typename Int>
struct safe
{
    static constexpr auto max_value = std::numeric_limits<Int>::max();
    static constexpr auto min_value = std::numeric_limits<Int>::min();

    safe() noexcept = default;

    constexpr safe(Int value) noexcept
        : m_value(value)
    {}

    constexpr explicit safe(status st) noexcept
        : m_status(st)
    {}

    Int value() const
    {
        // FIXME: throw if not valid?
        return m_value;
    }

    bool normal() const noexcept { return m_status == status::normal; }
    bool plus_infinity() const noexcept { return m_status == status::plus_infinity; }
    bool minus_infinity() const noexcept { return m_status == status::minus_infinity; }
    bool invalid() const noexcept { return m_status == status::invalid; }
    status get_status() const noexcept { return m_status; }

private:
    Int m_value = 0;
    status m_status = status::normal;
};

template<typename Int>
safe<Int> add(safe<Int> a, safe<Int> b)
{
    if (a.invalid() || b.invalid())
        return safe<Int>{status::invalid};

    if (b.value() >= 0)
    {
        if (a.value() > safe<Int>::max_value - b.value())
            return safe<Int>{status::plus_infinity};
    }
    else
    {
        if (a.value() < safe<Int>::min_value - b.value())
            return safe<Int>{status::plus_infinity};
    }
    return a.value() + b.value();
}

template<typename Int>
safe<Int> operator+(safe<Int> a, safe<Int> b)
{
    // This implementation is much smaller and faster.
    Int s;
    if (__builtin_add_overflow(a.value(), b.value(), &s))
        return safe<Int>{status::plus_infinity};
    return s;
}

template<typename Int>
safe<Int> operator+(Int a, safe<Int> b)
{
    return safe<Int>{a} + b;
}

template <typename Int>
safe<Int> operator+(safe<Int> a, Int b)
{
    return a + safe<Int>{b};
}

template<typename Int>
bool operator==(safe<Int> a, safe<Int> b)
{
    if (a.normal() && b.normal())
        return a.value() == b.value();

    if (a.plus_infinity() && (b.minus_infinity() || b.normal()))
        return false;

    if (b.plus_infinity() && (a.minus_infinity() || a.normal()))
        return false;

    throw bad_operation{};
}

template<typename Int>
bool operator==(safe<Int> a, Int b)
{
    return a == safe<Int>{b};
}

template<typename Int>
bool operator!=(safe<Int> a, safe<Int> b) { return !(a == b); }

template<typename Int>
bool operator<(safe<Int> a, safe<Int> b)
{
    switch (a.get_status())
    {
    case status::normal:
        switch (b.get_status())
        {
        case status::normal:
            return a.value() < b.value();
        case status::plus_infinity:
            return true;
        case status::minus_infinity:
            return false;
        case status::invalid:
            throw bad_operation{};
        }
    case status::plus_infinity:
        switch (b.get_status())
        {
        case status::normal:
            return false;
        case status::plus_infinity:
            throw bad_operation{};
        case status::minus_infinity:
            return false;
        case status::invalid:
            throw bad_operation{};
        }
    case status::minus_infinity:
        switch (b.get_status())
        {
        case status::normal:
            return true;
        case status::plus_infinity:
            return true;
        case status::minus_infinity:
            throw bad_operation{};
        case status::invalid:
            throw bad_operation{};
        }
    default:
        throw bad_operation{};
    }
}

template<typename Int>
bool operator<=(safe<Int> a, safe<Int> b) { return a < b || a == b; }

template<typename Int>
bool operator>=(safe<Int> a, safe<Int> b) { return !(a < b); }

template<typename Int>
bool operator>(safe<Int> a, safe<Int> b) { return !(a <= b); }

using safe_int = safe<int>;

template safe_int operator+<int>(safe_int a, safe_int b);
template safe_int add<int>(safe_int a, safe_int b);

}
