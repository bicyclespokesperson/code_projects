#!/usr/bin/env python3
#
# Imports historical bitcoin data from a csv file

import csv

def get_data(filename):
    '''
    Returns a list of bitcoin hourly price

    List is sorted by time with most recent price last
    '''
    with open(filename, mode='r', encoding='utf-8') as infile:
        price_reader = csv.reader(infile, delimiter=',')
        next(price_reader, None) # Skip headers
        return [float(row[2]) for row in price_reader][::-1]

if __name__ == '__main__':
    print('\n'.join(get_data('../data/Coinbase_BTCUSD_1h.csv')))

