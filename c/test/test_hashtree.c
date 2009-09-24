#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <google/cmockery.h>

#include "hashtable.h"

#include "unit_testing.h"


void test_tree_create(void **state)
{}


void test_tree_insert(void **state)
{}


void test_tree_search(void **state)
{}


void test_tree_free(void **state)
{}


int main(int argc, char* argv[]) {
    const UnitTest tests[] = {
        unit_test(test_tree_create),
        unit_test(test_tree_insert),
        unit_test(test_tree_search),
        unit_test(test_tree_free),
    };
    return run_tests(tests);
}

