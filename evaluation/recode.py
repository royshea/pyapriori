#!/usr/bin/python

if __name__ == '__main__':

    data_file = open("intermediate_200.txt")
    for data in data_file.readline().split():
        print hash(data) % (2 ** 16)
    data_file.close()
