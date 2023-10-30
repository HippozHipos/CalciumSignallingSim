import pygame
import pygame_gui
import sys

LEFT_MOUSE = 0
MIDDLE_MOUSE = 1
RIGHT_MOUSE = 2

class Window:
    def __init__(self, name, width, height):
        pygame.display.set_caption(name)
        self.width = width
        self.height = height
        self.display = pygame.display.set_mode((width, height))
        self.keyPressed = [False for i in range(255)]
        self.keyHeld = [False for i in range(255)]
        self.mousePressed = [False for i in range(3)]
        self.mouseHeld = [False for i in range(3)]
        self.mouseReleased = [False for i in range(3)]
        self.mouseScroll = 0
        self.clock = pygame.time.Clock()
        self.manager = pygame_gui.UIManager((width, height), "pygame_gui_theme.json")
        self.events = None
        self.deltaTime = 0

    def fill(self, r, g, b):
        self.display.fill((r, g, b, 255))

    def __UpdateKeyPressed(self, key):
        if key <= 255:
            if (pygame.key.get_pressed()[key]):
                self.keyPressed[key] = True

    def __UpdateMousePressed(self, button):
        button -= 1
        if (button < 3):
            if (pygame.mouse.get_pressed()[button]):
                self.mousePressed[button] = True

    def __UpdateMouseHeld(self, button):
        button -= 1
        if (button < 3):
            self.mouseHeld[button] = True

    def __UpdateMouseRelease(self, button):
        button -= 1
        if (button < 3):
            self.mouseReleased[button] = True

    def __UpdateMouseHeldRelease(self, button):
        button -= 1
        if (button < 3):
            self.mouseHeld[button] = False

    def update(self):
        self.keyPressed = [False for i in range(255)]
        self.keyHeld = pygame.key.get_pressed()
        self.mousePressed = [False for i in range(3)]
        self.mouseReleased = [False for i in range(3)]
        self.mouseScroll = 0
        self.deltaTime = self.clock.tick(120) / 1000.0
        self.events = pygame.event.get()
        for event in self.events:
            if event.type == pygame.KEYDOWN:
                self.__UpdateKeyPressed(event.key)
            if event.type == pygame.MOUSEBUTTONDOWN:
                self.__UpdateMousePressed(event.button)
                self.__UpdateMouseHeld(event.button)
            if event.type == pygame.MOUSEBUTTONUP:
                self.__UpdateMouseHeldRelease(event.button)
                self.__UpdateMouseRelease(event.button)
            if event.type == pygame.MOUSEWHEEL:
                self.mouseScroll = event.precise_y
            self.manager.process_events(event)
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
        self.manager.update(self.deltaTime)
        self.manager.draw_ui(self.display)
        pygame.display.flip()


