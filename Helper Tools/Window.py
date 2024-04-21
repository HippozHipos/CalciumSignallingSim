import pygame
import sys

LEFT_MOUSE = 0
MIDDLE_MOUSE = 1
RIGHT_MOUSE = 2

class Window:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.display = pygame.display.set_mode((width, height))
        self.keyPressed = [False for i in range(255)]
        self.keyHeld = [False for i in range(255)]
        self.mousePressed = [False for i in range(3)]
        self.mouseHeld = [False for i in range(3)]

    def __UpdateKeyPressed(self, key):
        if (pygame.key.get_pressed()[key]):
            self.keyPressed[key] = True

    def __UpdateMousePressed(self, button):
        button -= 1
        if (button < 3):
            if (pygame.mouse.get_pressed()[button]):
                self.mousePressed[button] = True

    def update(self):
        self.keyPressed = [False for i in range(255)]
        self.keyHeld = pygame.key.get_pressed()
        self.mousePressed = [False for i in range(3)]
        self.mouseHeld = pygame.mouse.get_pressed()
        for event in pygame.event.get():
            if event.type == pygame.KEYDOWN:
                self.__UpdateKeyPressed(event.key)
            if event.type == pygame.MOUSEBUTTONDOWN:
                self.__UpdateMousePressed(event.button)
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
        
        pygame.display.flip()


