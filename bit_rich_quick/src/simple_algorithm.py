#!/usr/bin/env python3

from trading_algorithm import *

class SimpleAlgorithm(TradingAlgorithm):
    '''
    Simple, benevolent bitcoin trader
    '''

    BUY_PERCENT = 5 # Buy after this much of a drop from previous ... hour?
    SELL_PERCENT = 5 # Sell after this much of an increase from buy price

    def __init__(self, initial_price, initial_capital):
        super().__init__(initial_price, initial_capital)
        self.purchase_price = 0.0

        # Access to:
        # self.hourly_price()
        # self.previous_price()
        # self.liquid_capital()
        # self.invested_capital()
        # self.initial_capital()

    def make_decision(self):
        if self.invested_capital() > 0.0:
            if self.hourly_price() >= self.purchase_price * ((100 + self.SELL_PERCENT) / 100.0):
                return (Action.SELL, self.invested_capital())
        elif self.liquid_capital() > 0:
            if  self.hourly_price() <= self.previous_price() * ((100 - self.BUY_PERCENT) / 100.0):
                self.purchase_price = self.hourly_price()
                return (Action.BUY, self.initial_capital())
        return (Action.HOLD, 0.0)

