import pygame
from Window import LEFT_MOUSE
import os
import math

class Viewport:
    def __init__(self, window, x, y, width, height):
        self.window = window
        self.x = x
        self.y = y
        self.width = width
        self.height = height
        self.zoomSensitivity = 5
        self.curPath = os.path.dirname(os.path.abspath(__file__))
        self.scale = [1, 1]
        self.panOffset = [0, 0]
        self.lastFrameMousePos = [0, 0]

    def drawElipse(self, x, y, w, h, color, width = 0):
        pygame.draw.ellipse(self.window.display, 
        color, (*self.toViewport(x, y), w * self.scale[0] , h * self.scale[1]), 
        width)

    def drawRect(self, x, y, w, h, color, width = 0, boarder = 1):
        pygame.draw.rect(self.window.display, 
        color, (*self.toViewport(x, y), w * self.scale[0], h * self.scale[1]),
        width, boarder)

    def drawLine(self, start, end, color, width = 0):
        pygame.draw.line(self.window.display, 
        color, self.toViewport(start[0], start[1]),
        self.toViewport(end[0], end[1]),
        width)

    def drawText(self, x, y, text, color, size):
        font = pygame.freetype.Font(self.curPath + "\\Assets\\Font\\open-sans\\OpenSans-Regular.ttf", size * self.scale[0])
        font.render_to(self.window.display, self.toViewport(x, y), text, color)

    def collidePoint(self, rect, point):
        r = pygame.Rect(*self.toViewport(rect[0], rect[1]), 
                           rect[2] * self.scale[0], rect[3] * self.scale[1]
                           )
        return r.collidepoint(point[0], point[1])
    
    def collideRectLine(self, rect, lineStart, lineEnd):
         r = pygame.Rect(rect[0], rect[1], 
                           rect[2] * self.scale[0], rect[3] * self.scale[1]
                           )

         return r.clipline(lineStart, lineEnd)
    
    def collidePointCircle(self, pos, radius, point):
        return math.dist(point, self.toViewport(*pos)) < radius * self.scale[0]

    def makeDragable(self):
        pygame.mouse.set_cursor(pygame.SYSTEM_CURSOR_ARROW)
        if self.window.keyHeld[pygame.K_q] and self.window.mouseHeld[LEFT_MOUSE]:
            if pygame.Rect(self.x, self.y, self.width, self.height).collidepoint(pygame.mouse.get_pos()):
                pygame.mouse.set_cursor(pygame.SYSTEM_CURSOR_SIZEALL)
                mousePos = pygame.mouse.get_pos()
                scale = [1 / self.scale[0], 1 / self.scale[1]]
                self.panOffset[0] += (self.lastFrameMousePos[0] - mousePos[0]) * scale[0] if scale[0] else 1
                self.panOffset[1] += (self.lastFrameMousePos[1] - mousePos[1]) * scale[1] if scale[1] else 1
                
        self.lastFrameMousePos = pygame.mouse.get_pos()

    def makeZoomableToMousePos(self):
        if pygame.Rect(self.x, self.y, self.width, self.height).collidepoint(pygame.mouse.get_pos()):
            mouseBeforeZoom = self.toWorld(*pygame.mouse.get_pos())
            rate = self.window.mouseScroll * self.zoomSensitivity * self.window.deltaTime
            self.scale[0] += rate; 
            self.scale[1] += rate; 
            self.scale[0] = min(20, max(0.1, self.scale[0]))
            self.scale[1] = min(20, max(0.1, self.scale[1]))
            mouseAfterZoom = self.toWorld(*pygame.mouse.get_pos())
            self.panOffset[0] += mouseBeforeZoom[0] - mouseAfterZoom[0]
            self.panOffset[1] += mouseBeforeZoom[1] - mouseAfterZoom[1]
        
    def drawOutline(self):
        pygame.draw.rect(self.window.display, (155, 155, 155), (self.x, self.y, self.width, self.height), 1)

    def update(self):
        self.makeDragable()
        self.makeZoomableToMousePos()
        self.drawOutline()
        if self.window.keyPressed[pygame.K_SPACE]:
            self.panOffset = [0, 0]
            self.scale = [1, 1]

    def getMousePos(self):
         mx, my = pygame.mouse.get_pos()
         mx /= self.scale[0]; my /= self.scale[1]
         mx += self.panOffset[0]; my += self.panOffset[1]
         mx -= self.x; my -= self.y
         return (mx, my)

    def toViewport(self, x, y):
        return ((x + self.x - self.panOffset[0]) * self.scale[0],
                 (y + self.y - self.panOffset[1]) * self.scale[1]) 

    def toWorld(self, x, y):
        scalex = self.scale[0] if self.scale[0] else 0.000001
        scaley = self.scale[1] if self.scale[1] else 0.000001
        return (((x + self.x) / scalex ) + self.panOffset[0],
                 ((y + self.y) / scaley) + self.panOffset[1])