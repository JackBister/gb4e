#pragma once

#include "greatest.h"

#include "Common.hh"

TEST FindFirstSet_0()
{
    using namespace gb4e;

    ASSERT_EQ(0, FindFirstSet((u32)0));

    PASS();
}

TEST FindFirstSet_1()
{
    using namespace gb4e;

    ASSERT_EQ(1, FindFirstSet((u32)1));

    PASS();
}

TEST FindFirstSet_2()
{
    using namespace gb4e;

    ASSERT_EQ(2, FindFirstSet((u32)2));

    PASS();
}

SUITE(Common_test)
{
    RUN_TEST(FindFirstSet_0);
    RUN_TEST(FindFirstSet_1);
    RUN_TEST(FindFirstSet_2);
}
