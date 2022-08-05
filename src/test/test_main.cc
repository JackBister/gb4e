
#include <stdbool.h>

#include "greatest.h"

#include "Common_test.hh"
#include "Cpu_test.hh"
#include "Gpu_test.hh"
#include "Instruction_test.hh"
#include "Test_ROMs.hh"

#pragma warning(push)
#pragma warning(disable : 4996)
#pragma warning(disable : 4551)
GREATEST_MAIN_DEFS();
#pragma warning(pop)

int main(int argc, char ** argv)
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(Instruction_test);
    RUN_SUITE(Cpu_test);
    RUN_SUITE(Gpu_test);
    RUN_SUITE(Common_test);
    RUN_SUITE(Test_ROMs);

    GREATEST_MAIN_END();
}