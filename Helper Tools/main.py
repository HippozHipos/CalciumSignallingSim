import pygame
from Window import Window
from UI import UI, TopUIPannel, PackedTissueUIPannel, CellUIPannel, GraphTissueUIPannel
from UIViewport import PackedTissueViewport, GraphTissueViewport

class Application:
    def __init__(self):
        self.window = Window("Tissue Maker", 1200, 600)
        self.leftUiGap = 250
        self.topUiGap = 50
        
        self.ui = UI(self.window.manager)

        self.topUiPannel = TopUIPannel((self.leftUiGap, 0, self.window.width - self.leftUiGap, self.topUiGap), 
                                       self.window, self.ui)
        
        self.packedTissueUiPannel = PackedTissueUIPannel((0, 0, self.leftUiGap, self.window.height),
                                                self.window, self.ui)

        self.graphTissueUiPannel = GraphTissueUIPannel((0, 0, self.leftUiGap, self.window.height),
                                                self.window, self.ui)

        self.cellUiPannel = CellUIPannel((0, 0, self.leftUiGap, self.window.height),
                                                self.window, self.ui)

        self.viewportRect = pygame.Rect(self.leftUiGap, self.topUiGap, 
                        self.window.width - self.leftUiGap, self.window.height - self.topUiGap)

        self.packedTissueViewport = PackedTissueViewport(self.window,
                self.viewportRect.x, self.viewportRect.y,
                self.viewportRect.w, self.viewportRect.h)
        
        self.graphTissueViewport = GraphTissueViewport(self.window,
                self.viewportRect.x, self.viewportRect.y,
                self.viewportRect.w, self.viewportRect.h)

        self.pannels = [self.packedTissueUiPannel, self.graphTissueUiPannel, self.cellUiPannel, 
                        self.packedTissueViewport, self.graphTissueViewport, None]

        self.currentPannel = self.packedTissueUiPannel
        self.currentViewport = self.packedTissueViewport
        self.switchTab(None, self.currentPannel, 0)#make curent pannel active tab
        
    def switchTab(self, old, new, index):
        self.currentViewport = self.pannels[index + 3]#3 because look at pannels array
        if old is not None:
            old.hide()
        if new is not None:
            new.show()

    def hanldeTabSwitching(self):
        i = 0
        for _, value in self.topUiPannel.elements.items():
            if value.check_pressed():
                self.switchTab(self.currentPannel, self.pannels[i], i)
                self.currentPannel = self.pannels[i]
            i += 1

    def run(self):
        while (True): 
            self.window.update()

            self.window.fill(25, 25, 25)
            if self.currentViewport is not None:
                self.currentViewport.viewport.update()
            self.hanldeTabSwitching()

            if self.currentPannel == self.cellUiPannel:
                self.cellUiPannel.onApply()
                self.cellUiPannel.onSave()
                self.cellUiPannel.onLoad()

            if self.currentViewport is not None:
                self.currentViewport.draw()

            if self.currentPannel == self.packedTissueUiPannel:
                xy = self.packedTissueViewport.makeCellChooseable()
                    
                self.packedTissueUiPannel.onApply()
                self.packedTissueUiPannel.onSave()
                self.packedTissueUiPannel.onLoad()
                self.packedTissueUiPannel.onRefresh()
                self.packedTissueUiPannel.onGenerate()
                self.packedTissueUiPannel.updateActivePropertiesInput(xy)
                update = self.packedTissueUiPannel.handleCellDragDrop(xy)
                if update:
                    self.packedTissueViewport.currentCell = xy

                self.packedTissueViewport.shape = self.packedTissueUiPannel.tissueShape
                self.packedTissueViewport.cells = self.packedTissueUiPannel.cells
                self.packedTissueViewport.layer = self.packedTissueUiPannel.layer

            if self.currentPannel == self.graphTissueUiPannel:
                vmpos = self.graphTissueViewport.viewport.getMousePos()
                update = self.graphTissueUiPannel.handleCellDragDrop(vmpos)

                cell = self.graphTissueViewport.mouseInCell()
                self.graphTissueViewport.drawMouseHoverCellOutline(cell)
                self.graphTissueViewport.setCurrentCellOnClick(cell)
                self.graphTissueViewport.updateSelectedConnection(vmpos)
                self.graphTissueViewport.makeCellDragable(cell)
                
                self.graphTissueUiPannel.handleAddingConnections(cell)
                self.graphTissueUiPannel.onSave()
                self.graphTissueUiPannel.onRefresh()
                self.graphTissueUiPannel.onGenerate()
                if self.graphTissueUiPannel.onLoad():
                    self.graphTissueViewport.currentCell = None
                    self.graphTissueViewport.selectedConnecetion = None

                self.graphTissueViewport.cells = self.graphTissueUiPannel.cells
                self.graphTissueUiPannel.currentCell = self.graphTissueViewport.currentCell
                self.graphTissueUiPannel.currentConnection = self.graphTissueViewport.selectedConnecetion
            
            self.topUiPannel.drawBackground()
            self.currentPannel.drawBackground()

def main():
    app = Application()
    app.run()

if __name__ == "__main__":
    pygame.init()
    main()

