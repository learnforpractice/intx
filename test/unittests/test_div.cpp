// intx: extended precision integer library.
// Copyright 2018 Pawel Bylica.
// Licensed under the Apache License, Version 2.0. See the LICENSE file.

#include <div.hpp>

#include <gtest/gtest.h>

using namespace intx;

TEST(div, normalize)
{
    uint512 u;
    uint512 v = 1;
    auto na = div::normalize(u, v);
    EXPECT_EQ(na.shift, 31);
    EXPECT_EQ(na.num_denominator_words, 1);
    EXPECT_EQ(na.num_numerator_words, 0);
    EXPECT_EQ(na.numerator[0], 0);
    EXPECT_EQ(na.numerator[1], 0);
    EXPECT_EQ(na.numerator[16], 0);
    EXPECT_EQ(na.denominator[0], 1u << 31);
    EXPECT_EQ(na.denominator[1], 0);

    u = uint512{1313, 1414};
    v = uint512{1212, 12};
    na = div::normalize(u, v);
    EXPECT_EQ(na.shift, 28);
    EXPECT_EQ(na.num_denominator_words, 9);
    EXPECT_EQ(na.num_numerator_words, 9);
    EXPECT_EQ(na.numerator[0], 1313u << 28);
    EXPECT_EQ(na.numerator[1], 1313u >> (32 - 28));
    EXPECT_EQ(na.numerator[2], 0);
    EXPECT_EQ(na.numerator[8], 1414u << 28);
    EXPECT_EQ(na.numerator[na.num_numerator_words], 1414u >> (32 - 28));
    EXPECT_EQ(na.denominator[0], 1212u << 28);
    EXPECT_EQ(na.denominator[1], 1212u >> (32 - 28));
    EXPECT_EQ(na.denominator[2], 0);
    EXPECT_EQ(na.denominator[8], 12u << 28);
    EXPECT_EQ(na.denominator[9], 0);

    u = shl(uint512{3}, 510);
    v = uint512{uint256{1, 0xffffffff}, 0};
    na = div::normalize(u, v);
    EXPECT_EQ(na.shift, 0);
    EXPECT_EQ(na.num_denominator_words, 5);
    EXPECT_EQ(na.num_numerator_words, 16);
    EXPECT_EQ(na.numerator[0], 0);
    EXPECT_EQ(na.numerator[1], 0);
    EXPECT_EQ(na.numerator[2], 0);
    EXPECT_EQ(na.numerator[8], 0);
    EXPECT_EQ(na.numerator[15], 3 << 30);
    EXPECT_EQ(na.numerator[16], 0);
    EXPECT_EQ(na.denominator[0], 1);
    EXPECT_EQ(na.denominator[1], 0);
    EXPECT_EQ(na.denominator[2], 0);
    EXPECT_EQ(na.denominator[3], 0);
    EXPECT_EQ(na.denominator[4], 0xffffffff);
    EXPECT_EQ(na.denominator[5], 0);

    u = shl(uint512{7}, 509);
    v = uint512{uint256{1, 0x3fffffff}, 0};
    na = div::normalize(u, v);
    EXPECT_EQ(na.shift, 2);
    EXPECT_EQ(na.num_denominator_words, 5);
    EXPECT_EQ(na.num_numerator_words, 16);
    EXPECT_EQ(na.numerator[14], 0);
    EXPECT_EQ(na.numerator[15], 1 << 31);
    EXPECT_EQ(na.numerator[16], 3);
    EXPECT_EQ(na.denominator[0], 4);
    EXPECT_EQ(na.denominator[1], 0);
    EXPECT_EQ(na.denominator[2], 0);
    EXPECT_EQ(na.denominator[3], 0);
    EXPECT_EQ(na.denominator[4], 0xfffffffc);
    EXPECT_EQ(na.denominator[5], 0);
}

//static const char* div_tests[][2] = {
//    {"114985996462255550791716983618628317438843204990487060894154871723529666670495818727603768585"
//     "47580936454739751935356826554181006922789206592312369735468831",
//        "34423876519961882806530261309917882426503998231435401563510253050001326285506"}};
//
//TEST(div, udivrem_512)
//{
//    for (auto& t : div_tests)
//    {
//        auto u = from_string_512(t[0]);
//        auto v = from_string_512(t[1]);
//
//        uint512 q, r;
//        std::tie(q, r) = div::udivrem(u, v);
//        EXPECT_EQ(q, 1);
//        EXPECT_EQ(r, 1);
//    }
//}