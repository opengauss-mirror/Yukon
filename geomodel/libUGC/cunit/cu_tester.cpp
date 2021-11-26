#include <stdio.h>
#include <stdlib.h>
#include "cu_tester.h"
#include "CUnit/Basic.h"

extern void example_suite_setup();

YK_SuiteSetup setupfuncs[] =
{
        example_suite_setup,
        NULL
};


int main(int argc, char *argv[])
{
    if (CU_initialize_registry())
    {
        fprintf(stderr, " Initialization of Test Registry failed. ");
        exit(EXIT_FAILURE);
    }
    else
    {
        YK_SuiteSetup *setupfunc = setupfuncs;
        while (*setupfunc)
        {
            (*setupfunc)();
            setupfunc++;
        }
        CU_basic_run_tests();
        CU_cleanup_registry();

        return CU_get_error();
    }
}