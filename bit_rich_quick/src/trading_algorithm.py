#!/usr/bin/env python3

from enum import Enum

class Action(Enum):
    HOLD = 1
    BUY = 2
    SELL = 3

class TradingAlgorithm:
    '''
    Abstract class representing a bitcoin trader

    Override make_decision to use

    Treat protected variables as read-only, this base class will update them.

    Make sure to call super().__init__(initial_price, initial_capital)
    in the child's __init__ method.
    '''

    def __init__(self, initial_price, initial_capital):
        self.__initial_price = initial_price
        self.__invested_capital = 0
        self.__liquid_capital = initial_capital
        self.__initial_capital = initial_capital
        self.__buy_count = 0
        self.__sell_count = 0
        self.__previous_price = initial_price
        self.__hourly_price = initial_price

    def call_decision(self, hourly_price):
        '''
        Called by simulator, do not override
        '''
        self.__invested_capital *= (hourly_price / self.__previous_price)
        self.__hourly_price = hourly_price
        decision, amount = self.make_decision()
        if decision == Action.BUY:
            if amount > self.__liquid_capital:
                raise RuntimeError('Error: Spending too much money')
            self.__invested_capital += amount
            self.__liquid_capital -= amount
            self.__buy_count += 1
        elif decision == Action.SELL:
            if amount > self.__invested_capital:
                raise RuntimeError('Error: Selling too much money')
            self.__invested_capital -= amount
            self.__liquid_capital += amount
            self.__sell_count += 1
        self.__previous_price = hourly_price
        return decision, amount

    def hourly_price(self):
        return self.__hourly_price

    def previous_price(self):
        return self.__previous_price

    def initial_capital(self):
        return self.__initial_capital

    def liquid_capital(self):
        return self.__liquid_capital

    def invested_capital(self):
        return self.__invested_capital

    def buy_sell_count(self):
        return (self.__buy_count, self.__sell_count)

    def make_decision(self):
        '''
        Override this method

        hourly_price - The current price
        invested_capital - The amount currently invested, reflecting market changes

        Return a tuple of (decision, amount to buy/sell)
        '''
        return (Action.HOLD, 0.0)



