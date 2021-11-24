
#ifndef YK_CUNIT_H
#define YK_CUNIT_H

#define YK_ADD_TEST(suite, testfunc) CU_add_test(suite, #testfunc, testfunc)
typedef void (*YK_SuiteSetup)(void);

#endif 