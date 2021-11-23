#include <CUnit/Basic.h>
#include "cu_tester.h"
// 包含其他需要测试函数的头文件

/**
 * @brief 开始测试前需要准备的环境
 * 
 * @return int 
 */
static int init_example_suite(void)
{

    return 0;
}

/**
 * @brief 测试结束后，需要清理环境
 * 
 * @return int 
 */
static int clean_example_suite(void)
{

    return 0;
}

/**
 * @brief 测试示例函数
 * 
 * @param a 
 * @param b 
 * @return int 
 */
static int _max(int a, int b)
{
    return a > b ? a : b;
}

static void test_max(void)
{
    CU_ASSERT_EQUAL(_max(5, 6), 6);
    CU_ASSERT_EQUAL(_max(9, 6), 9);
}

void example_suite_setup(void);
void example_suite_setup(void)
{
	CU_pSuite suite = CU_add_suite("example_test", init_example_suite, clean_example_suite);
	UGC_ADD_TEST(suite,test_max);
}