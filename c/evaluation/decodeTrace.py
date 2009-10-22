#!/usr/bin/python

import sys


if __name__ == '__main__':

    # Create the mapping from names to numbers
    reverse_map = {}
    dict_file = open("callDict.txt")
    for pair in dict_file.readlines():

        # Verify that we have one function name / ID pair per-line
        assert(len(pair.split()) == 2)

        (name, id) = pair.split()
        reverse_map[int(id)] = name
    dict_file.close()

    # De-encode the sets
    set_file = open(sys.argv[1])
    for data in set_file.readlines():

        for id in data.split():
            print reverse_map[int(id)],
        print

    set_file.close()
