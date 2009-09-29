#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <google/cmockery.h>
#include <stdio.h>

#include "apriori.h"

#include "unit_testing.h"

char tmp_file_name[] = "test_apriori_data.tmp";

void test_read_uint16_list(void **state)
{
    FILE *fid;
    List *list;
    int i;

    /* Basic test. */
    fid = fopen(tmp_file_name, "w");
    fprintf(fid, "0 1 2 3");
    fclose(fid);
    list = read_uint16_list(tmp_file_name);
    assert_int_equal(ll_length(list), 4);
    ll_sort(list);
    for (i=0; i<4; i++)
    {
        uint16_t *data;
        data = ll_get_nth(list, i);
        assert_int_equal(*data, i);
    }
    ll_free(list);

    /* More complex data file formatting. */
    fid = fopen(tmp_file_name, "w");
    fprintf(fid, "   0\t 1\n2 3  ");
    fclose(fid);
    list = read_uint16_list(tmp_file_name);
    assert_int_equal(ll_length(list), 4);
    ll_sort(list);
    for (i=0; i<4; i++)
    {
        uint16_t *data;
        data = ll_get_nth(list, i);
        assert_int_equal(*data, i);
    }
    ll_free(list);

    remove(tmp_file_name);
}


void test_make_transactions_fixed_width(void **state)
{
}


int main(int argc, char* argv[]) {
    const UnitTest tests[] = {
        unit_test(test_read_uint16_list),
        unit_test(test_make_transactions_fixed_width),
    };
    return run_tests(tests);
}
