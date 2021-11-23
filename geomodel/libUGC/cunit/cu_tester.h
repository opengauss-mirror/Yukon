
#ifndef UGC_CUNIT_H
#define UGC_CUNIT_H

#define UGC_ADD_TEST(suite, testfunc) CU_add_test(suite, #testfunc, testfunc)
typedef void (*UGC_SuiteSetup)(void);

#endif 