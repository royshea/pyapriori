#!/usr/bin/python

if __name__ == '__main__':

    calls_file = open("eslTinyOSCalls.txt")
    for data in calls_file.readlines():

        # Verify that we have one function name per-line
        assert(len(data.split()) == 1)

        call = data.split()[0]
        if call == "-1":
            # Special case for the return function encoded as -1
            print "RETURN",
        elif call == "-2":
            # Special case for the log stream error encoded as -2
            print "ERROR",
        else:
            # General case
            print call,
        print hash(call) % (2 ** 16)
    calls_file.close()
