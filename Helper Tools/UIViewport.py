from Viewport import Viewport
import pygame
from Window import LEFT_MOUSE
import math

class PackedTissueViewport:
    def __init__(self, window, x, y, w, h):
        self.viewport = Viewport(window, x, y, w, h)
        self.currentLayer = 0
        self.currentCell = [0, 0]
        self.shape = [0, 0 ,0]
        self.cells = []
        self.cellSize = 100
        self.layerOffset = 100
        self.layer = 0

    def draw(self):
        self.drawLayerGird()
        self.drawSelectedCell()
        self.drawCellNames()

    def drawSelectedCell(self):
        self.viewport.drawRect(self.currentCell[0] * self.cellSize + self.layerOffset,
                            self.currentCell[1] * self.cellSize + self.layerOffset, self.cellSize, self.cellSize,
                                            (100, 255, 100), 5)

    def drawLayerGird(self):
        for x in range(self.shape[0] + 1):
            self.viewport.drawLine((x * self.cellSize + self.layerOffset, self.layerOffset), 
                    (x * self.cellSize + self.layerOffset, self.shape[1] * self.cellSize + self.layerOffset), 
                    (255, 255, 255), 2)
        
        for y in range(self.shape[1] + 1):
            self.viewport.drawLine((self.layerOffset, y * self.cellSize + self.layerOffset),
                    (self.shape[0] * self.cellSize + self.layerOffset, y * self.cellSize + self.layerOffset),
                      (255, 255, 255), 2)
            
    def drawCellNames(self):
        for x in range(self.shape[0]):
            for y in range(self.shape[1]):
                name = self.cells[self.calcIndex(x, y, self.layer)].name
                name = "Not Set" if name == "" else name
                
                font = pygame.font.Font(None, 24)
                text = font.render(name, True, (255, 255, 255))
                _, text_height = text.get_size()

                cell_y = y * self.cellSize + self.layerOffset
                y_centered = cell_y + (self.cellSize - text_height) // 2

                size = 150 / len(name) if len(name) != 1 else 50
                self.viewport.drawText(x * self.cellSize + self.layerOffset + 5,
                                        y_centered, name, (255, 255, 255), size)

    def mouseInCell(self):
        for x in range(self.shape[0]):
            for y in range(self.shape[1]):
                if self.viewport.collidePoint((x * self.cellSize + self.layerOffset, y * self.cellSize + self.layerOffset,
                                                 self.cellSize, self.cellSize), 
                                              pygame.mouse.get_pos()):
                    return (x, y)
        return None

    def makeCellChooseable(self):
        mp = pygame.mouse.get_pos()
        if mp[0] > self.viewport.x and mp[1] > self.viewport.y:
            xy = self.mouseInCell()
            if xy is not None:
                x, y = xy
                self.viewport.drawRect(x * self.cellSize + self.layerOffset,
                                        y * self.cellSize + self.layerOffset, self.cellSize, self.cellSize,
                                        (255, 255, 0), 5)
                if self.viewport.window.mousePressed[LEFT_MOUSE]:
                    self.currentCell = [x, y]
            return xy
        return None

    def calcIndex(self, x, y, z):
        return (z * self.shape[0] * self.shape[1]) + y * self.shape[0] + x
    
class GraphTissueViewport:
    def __init__(self, window, x, y, w, h):
        self.viewport = Viewport(window, x, y, w, h)
        self.cells = []
        self.cellSize = 100
        self.currentCell = None
        self.selectedConnecetion = None
        self.hoveredConnection = None

    def draw(self):
        self.drawConnectionLines()
        self.drawCells()
        self.drawCurrentCell()

    def drawCurrentCell(self):
        if self.currentCell is not None:
            offset = self.currentCell.radius / 2 
            self.viewport.drawElipse(self.currentCell.position[0] - offset, 
                                        self.currentCell.position[1] - offset,
                                        self.currentCell.radius, self.currentCell.radius, 
                                        (100, 255, 100), 5)

    def drawCells(self):
        for cell in self.cells:
            font = pygame.font.Font(None, 24)
            text = font.render(cell.name, True, (40, 40, 40))
            text_width, text_height = text.get_size()

            max_dimension = max(text_width, text_height)

            cell.radius = max_dimension * 1.7

            offset = cell.radius / 2     
            self.viewport.drawElipse(cell.position[0] - offset,
                                      cell.position[1] - offset, 
                                      cell.radius, cell.radius, (120, 100, 80))

            x = cell.position[0] + (cell.radius - text_width) // 2
            y = cell.position[1] + (cell.radius - text_height) // 2

            x -= len(cell.name) * 2 

            self.viewport.drawText(x - offset, y - offset, cell.name, (30, 20, 10), 24)

    def drawMouseHoverCellOutline(self, cell):
        if cell is not None:
            offset = cell.radius / 2 
            self.viewport.drawElipse(cell.position[0] - offset, 
                                     cell.position[1] - offset,
                                     cell.radius, cell.radius, 
                                     (255, 255, 0), 5)
            
    def drawConnectionLines(self):
        for cell in self.cells:
            for con in cell.connections:
                self.viewport.drawLine(cell.position, con.position, (255, 255, 255), 2)
                
        if self.selectedConnecetion is not None:
            self.viewport.drawLine(self.selectedConnecetion[0].position, 
                                   self.selectedConnecetion[1].position, (100, 200, 100), 4)
        if self.hoveredConnection is not None:
            self.viewport.drawLine(self.hoveredConnection[0].position, 
                                   self.hoveredConnection[1].position, (255, 255, 0), 3)


    def updateSelectedConnection(self, mpos):
        mp = pygame.mouse.get_pos()
        if mp[0] > self.viewport.x and mp[1] > self.viewport.y:
            found = False
            for cell in self.cells:
                for con in cell.connections:
                    size = 20/self.viewport.scale[0]
                    if self.viewport.collideRectLine((mpos[0] - size/2, mpos[1] - size/2, size, size), 
                                                    cell.position, con.position):
                        self.hoveredConnection = [cell, con]
                        found = True
                        break
                if found:
                    break
                else:
                    self.hoveredConnection = None

            if self.hoveredConnection is not None:
                if self.viewport.window.mousePressed[LEFT_MOUSE]:
                    self.selectedConnecetion = [cell, con]
                    for cell in self.cells:
                        for g in cell.gapJunctions:
                            g.hide()
                            g.disable()
                    index = self.selectedConnecetion[0].connections.index(con)
                    self.selectedConnecetion[0].gapJunctions[index].show()
                    self.selectedConnecetion[0].gapJunctions[index].enable()
        else:
            self.hoveredConnection = None
            
    def setCurrentCellOnClick(self, cell):
        mpos = pygame.mouse.get_pos()
        if mpos[0] > self.viewport.x and mpos[1] > self.viewport.y:
            if cell is not None and self.viewport.window.mousePressed[LEFT_MOUSE]:
                self.currentCell = cell
                for cell in self.cells:
                    if cell == self.currentCell:
                        cell.propertiesPanel.show()
                    else:
                        cell.propertiesPanel.hide()

    def makeCellDragable(self, cell):
        if cell is not None and self.viewport.window.mouseHeld[LEFT_MOUSE] and self.viewport.window.keyHeld[pygame.K_e]:
            cell.position = self.viewport.getMousePos()

    def mouseInCell(self):
        for cell in self.cells:
            if self.viewport.collidePointCircle(cell.position, cell.radius * 0.5,
                                                 pygame.mouse.get_pos()):
                return cell
        return None