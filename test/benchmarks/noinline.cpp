// intx: extended precision integer library.
// Copyright 2019 Pawel Bylica.
// Licensed under the Apache License, Version 2.0.

#include <intx/intx.hpp>

using namespace intx;

auto add(const uint256& x, const uint256& y) noexcept
{
    return x + y;
}

auto sub(const uint256& x, const uint256& y) noexcept
{
    return x - y;
}

auto add(const uint512& x, const uint512& y) noexcept
{
    return x + y;
}

auto sub(const uint512& x, const uint512& y) noexcept
{
    return x - y;
}

auto reciprocal_2by1_noinline(uint64_t d) noexcept
{
    return reciprocal_2by1(d);
}

auto reciprocal_2by1_asm(uint64_t d)
{
    // https://godbolt.org/z/eJchZr

    uint64_t v;
    auto t = &intx::internal::reciprocal_table[0];
    asm(
    "movq	%1, %%rcx\n\t"
    "shrq	$55, %%rcx\n\t"
    "movzwl	-512(%%rcx,%2,2), %%ecx\n\t"

    "movq	%1, %%rsi\n\t"
    "movl	%%ecx, %%eax\n\t"
    "imull	%%ecx, %%ecx\n\t"
    "shrq	$24, %%rsi\n\t"
    "incq	%%rsi\n\t"
    "imulq	%%rsi, %%rcx\n\t"
    "shrq	$40, %%rcx\n\t"
    "sall	$11, %%eax\n\t"
    "decl	%%eax\n\t"
    "subl	%%ecx, %%eax\n\t"

    "movq	$0x1000000000000000, %%rcx\n\t"
    "imulq	%%rax, %%rsi\n\t"
    "subq	%%rsi, %%rcx\n\t"
    "imulq	%%rax, %%rcx\n\t"
    "salq	$13, %%rax\n\t"
    "shrq	$47, %%rcx\n\t"
    "addq	%%rax, %%rcx\n\t"

    "movq	%1, %%rsi\n\t"
    "shrq	%%rsi\n\t"
    "sbbq	%%rax, %%rax\n\t"
    "subq	%%rax, %%rsi\n\t"
    "imulq	%%rcx, %%rsi\n\t"
    "andq	%%rcx, %%rax\n\t"
    "shrq	%%rax\n\t"
    "subq	%%rsi, %%rax\n\t"
    "mulq	%%rcx\n\t"
    "salq	$31, %%rcx\n\t"
    "shrq	%%rdx\n\t"
    "addq	%%rdx, %%rcx\n\t"

    "movq	%1, %%rax\n\t"
    "mulq	%%rcx\n\t"
    "addq	%1, %%rax\n\t"
    "movq	%%rcx, %%rax\n\t"
    "addq	%1, %%rdx\n\t"
    "subq	%%rdx, %%rax\n\t"

    : "=a"(v)
    : "r"(d), "a"(t)
    );

    return v;
}
