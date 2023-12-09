import pygame
from Window import Window, LEFT_MOUSE, MIDDLE_MOUSE, RIGHT_MOUSE

class Application:
    def __init__(self):
        self.window = Window(600, 600)

    def run(self):
        while (True):
            self.window.update()
            

def main():
    app = Application()
    app.run()

if __name__ == "__main__":
    main()
