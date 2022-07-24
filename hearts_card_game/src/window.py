import tkinter as tk
import pydealer
import os
from PIL import ImageTk

IMAGES_DIRECTORY="images"

class HeartsWindow:

    def __init__(self):

        self.root = tk.Tk()
        self.width=800
        self.height=1000
        self.root.geometry(f"{self.width}x{self.height}")
        self.canvas = tk.Canvas(self.root, bg="white", height=self.height, width=self.width)
        self.canvas.pack()

        self.card_images = {}
        for card in pydealer.Deck():
            image_filename = str(card).lower().replace(' ', '_') + '.png'
            image_filename = os.path.join(IMAGES_DIRECTORY, image_filename)
            self.card_images[card] = ImageTk.PhotoImage(file=image_filename)

    def run_window(self):
        c = pydealer.Card("2", "spades")
        self.canvas.create_image(100, 200, image=self.card_images[c], anchor='ne')

        self.root.mainloop()


if __name__ == '__main__':
    w = HeartsWindow()
    w.run_window()

