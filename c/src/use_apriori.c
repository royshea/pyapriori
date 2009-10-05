#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <assert.h>

#include "apriori.h"
#include "uint16_list.h"

#define MAX_FILE_NAME_LENGTH 128

int main(int argc, char **argv)
{
    /* Key state for apriori. */
    float min_support;
    int width;
    char filename[MAX_FILE_NAME_LENGTH];
    List *frequent;

    /* Used by getopt_long. */
    int c;
    int option_index;
    static struct option long_options[] = {
        {"width", 1, 0, 0},
        {"support", 1, 0, 0},
        {0, 0, 0, 0}
    };
    int return_val;

    /* Flags to track that all options are properly set. */
    int width_set = 0;
    int min_support_set = 0;

    /* Parse options. */
    while (1)
    {
        c = getopt_long(argc, argv, "w:s:", long_options, &option_index);
        if (c == -1)
            break;

        switch(c)
        {
            case 0:
                if (option_index == 0)
                {
                    assert(width_set == 0);
                    width_set = 1;
                    return_val = sscanf(optarg, "%d", &width);
                    assert(return_val == 1);
                    assert(width > 0);
                }
                else if (option_index == 1)
                {
                    assert(min_support_set == 0);
                    min_support_set = 1;
                    return_val = sscanf(optarg, "%f", &min_support);
                    assert(return_val == 1);
                    assert(min_support > 0 && min_support < 1);
                }
                else
                    return -1;

                break;

            case 'w':
                assert(width_set == 0);
                width_set = 1;
                return_val = sscanf(optarg, "%d", &width);
                assert(return_val == 1);
                assert(width > 0);
                break;

            case 's':
                assert(min_support_set == 0);
                min_support_set = 1;
                return_val = sscanf(optarg, "%f", &min_support);
                assert(return_val == 1);
                assert(min_support > 0 && min_support < 1);
                break;

            default:
                break;
        }
    }

    /* Verify that width and min_support have been set. */
    assert(width_set == 1 && min_support_set == 1);

    /* Set the file name. */
    assert(argc - optind == 1);
    return_val = sscanf(argv[optind], "%100s", filename);
    assert(return_val == 1);

    /* Call into apriori. */
    frequent = apriori(filename, width, min_support);
    uint16_list_list_print(frequent);
    ll_free(frequent);

    return 0;
}

