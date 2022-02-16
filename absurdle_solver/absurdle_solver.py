#!/usr/bin/env python3

def read_word_list():

    # From: https://boardgames.stackexchange.com/questions/38366/latest-collins-scrabble-words-list-in-text-file
    words_filename = 'five_letter_words.txt'
    with open(words_filename, mode='r') as infile:
        return[line.strip() for line in infile]

def main():
    words = read_word_list()



if __name__ == '__main__':
    main()
