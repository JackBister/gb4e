
#include <stdbool.h>

#include "greatest.h"

#include "Instruction_test.hh"

#pragma warning(push)
#pragma warning(disable : 4996)
#pragma warning(disable : 4551)
GREATEST_MAIN_DEFS();
#pragma warning(pop)

int main(int argc, char ** argv)
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(Instruction_test);

    GREATEST_MAIN_END();
}