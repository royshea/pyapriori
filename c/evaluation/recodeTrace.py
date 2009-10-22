#!/usr/bin/python

import sys


if __name__ == '__main__':

    # Create the mapping from names to numbers
    map = {}
    dict_file = open("callDict.txt")
    for pair in dict_file.readlines():

        # Verify that we have one function name / ID pair per-line
        assert(len(pair.split()) == 2)

        (name, id) = pair.split()
        map[name] = int(id)
    dict_file.close()

    # Re-encode the trace
    trace_file = open(sys.argv[1])
    for token in trace_file.readline().split():

        # Special case for the RETURN and ERROR tokens
        if token == "-1": token = "RETURN"
        if token == "-2": token = "ERROR"

        print map[token]
