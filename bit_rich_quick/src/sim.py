#!/usr/bin/env python3
#
# Runs a simulation of bitcoin trading on historical data

import importer
from trading_algorithm import *
from simple_algorithm import SimpleAlgorithm

# Edit these settings
INITIAL_CAPITOL = 1000
TRADER = SimpleAlgorithm
DEBUG = False

INPUT_DATA_PATH = '../data/Coinbase_BTCUSD_1h.csv'

def main():
    '''
    Runs the simulation
    '''

    data = importer.get_data(INPUT_DATA_PATH)
    if not data:
        print('No data found')
        return

    current_price = data[0]
    trader = TRADER(current_price, INITIAL_CAPITOL)

    for hourly_price in data:
        _decision, _amount = trader.call_decision(hourly_price)
        if DEBUG:
            if _decision == Action.BUY:
                print(f'Buying ${_amount}')
            elif _decision == Action.SELL:
                print(f'Selling ${_amount}')

    final_capital = trader.liquid_capital() + trader.invested_capital()
    print(f'Initial capital: ${INITIAL_CAPITOL}')
    print(f'Final capital: ${final_capital}')
    print(f'Profit: ${final_capital - INITIAL_CAPITOL}')
    print(f'Percent change: {final_capital / INITIAL_CAPITOL * 100}')
    print(f'Times bought/sold: {trader.buy_sell_count()}')

if __name__ == '__main__':
    main()
