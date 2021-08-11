#!/usr/bin/env python3
#
# Given a json file of chess moves, see if you can identify the correct moves in an opening to respond to the
# opponents moves

import json
import random
import sys


def play_opening(filename):
    try:
        with open(filename, mode='r') as infile:
            moves = json.load(infile)
    except FileNotFoundError:
        print(f'File not found: {filename}', file=sys.stderr)
        sys.exit(-1)

    current = moves
    while True:
        whites_move = input("Enter White's Move: ")
        if whites_move in current:
            current = current[whites_move]
            if not current:
                print('Passed. No more moves in opening')
                break
            blacks_move = random.choice(list(current.keys()))
            print(f"Black's response: {blacks_move}")
            current = current[blacks_move]
        else:
            correct_move = random.choice(list(current.keys()))
            print(f'Failed: The correct move was: {correct_move}')
            break


if __name__ == '__main__':
    # filename = 'vienna_gambit_full.json'
    filename = 'vienna_gambit_section.json'
    play_opening(filename)
