import pygame_gui
import os
import pygame
import utils
import glob
import copy
from Window import LEFT_MOUSE
from hppgenerators import generatePackedTissuseHpp, generateGraphTissueHpp

class UI:
    def __init__(self, manager):
        self.manager = manager

    def button(self, text, x, y, w, h, container=None):
        return pygame_gui.elements.UIButton(pygame.Rect(x, y, w, h), text, self.manager,
                                            container=container)
    
    def text(self, text, x, y, w, h, container=None):
        return pygame_gui.elements.UILabel(pygame.Rect(x, y, w, h), text, self.manager, 
                                           container=container)
    
    def intTextInputBox(self, x, y, w, h, container=None, value="0"):
        box = pygame_gui.elements.UITextEntryLine(pygame.Rect(x, y, w, h),
                            self.manager,
                            container=container)
        box.set_allowed_characters("numbers")
        box.set_text(value)
        return box
    
    def textInputBox(self, x, y, w, h, container=None, value="0"):
        box = pygame_gui.elements.UITextEntryLine(pygame.Rect(x, y, w, h),
                            self.manager, container=container)
        box.set_text(value)
        return box
    
    def scrollablePannel(self, x, y, w, h, sw, sh):
        pannel = pygame_gui.elements.UIScrollingContainer(
            pygame.Rect(x, y, w, h), self.manager)
        pannel.set_scrollable_area_dimensions((sw, sh))
        return pannel
    
    def verticalScrollBar(self, container=None):
        return pygame_gui.elements.UIVerticalScrollBar(
            pygame.Rect(0, 0, 0, 0), 1,  
            self.manager,
            container=container,
        )
    
    def dropDownList(self, x, y, w, h, options):
        return pygame_gui.elements.UIDropDownMenu(
                    options_list=options,
                    starting_option="Layer 1",
                    relative_rect=pygame.Rect(x, y, w, h),
                    manager=self.manager)
    
    def popup(self, title, text, buttontext):
        return pygame_gui.windows.UIConfirmationDialog(pygame.Rect(300, 200, 350, 200), 
                text, self.manager, window_title=title, action_short_name=buttontext)
    
    def fileDialog(self, text, x, y, w, h, allowedSuffix):
        path = os.path.dirname(os.path.abspath(__file__))
        if os.path.exists(path + "\\working_files\\"):
            path += "\\working_files\\"
        return pygame_gui.windows.UIFileDialog(pygame.Rect(x, y, w, h),
            self.manager, text, allowed_suffixes=allowedSuffix, initial_file_path=path)

class TopUIPannel:
    def __init__(self, rect, window, ui):
        self.rect = pygame.Rect(*rect)
        self.window = window
        self.ui = ui
        self.elements = self.draw()        
    
    def drawBackground(self):
        pygame.draw.rect(self.window.display, (120, 120, 120),
                         (self.rect.x, self.rect.y, self.rect.w, self.rect.height))

    def draw(self):
        w = self.rect.width / 3
        return{
            "PackedTissue" : self.ui.button("Packed Tissue", self.rect.x, self.rect.y, w, self.rect.h),
            "GraphTissue" : self.ui.button("Graph Tissue", self.rect.x + w, self.rect.y, w, self.rect.h),
            "Cell" : self.ui.button("Cell", self.rect.x + w * 2, self.rect.y, w, self.rect.h)
        }
        
#scrollable field of int intputs that is used to input property 
#values for cells
class CellPropertyInputFields:
    def __init__(self, rect, ui):
        self.rect = pygame.Rect(*rect)
        self.ui = ui
        self.pannel = self.ui.scrollablePannel(0, 0, 0, 0, 0, 0)
        self.labels = []
        self.textBoxes = []
        
    def loadFromFile(self, file, nprops):
         self.pannel.kill()
         self.pannel = self.ui.scrollablePannel(0, 160, self.rect.width, self.rect.height, 
                                                            self.rect.width - 20, nprops * 40 + 30)
         
         self.textBoxes = []
         self.labels = []
         for i in range(nprops):
            self.textBoxes.append(self.ui.textInputBox(50, i * 40, 150, 30, 
                self.pannel, file.readline().strip()))
            
            self.labels.append(self.ui.text(str(i + 1), 10, i * 40, 30, 30, self.pannel))                
         self.pannel.rebuild()

    def apply(self, newProps):
        oldProps = len(self.textBoxes)
        if newProps != oldProps:
            self.pannel.kill()
            self.pannel = self.ui.scrollablePannel(self.rect.x, self.rect.y, self.rect.width, self.rect.height, 
                                                                self.rect.width - 20, newProps * 40)
        
            if newProps > oldProps:
                for i in range(oldProps):#necessary because properties pannel is updated
                    self.textBoxes[i] = self.ui.textInputBox(50, i * 40, 150, 30, 
                        self.pannel, self.textBoxes[i].get_text())
                    
                    self.labels[i] = self.ui.text(str(i + 1), 10, i * 40, 30, 30, self.pannel)

                for i in range(oldProps, newProps):
                    self.textBoxes.append(
                        self.ui.textInputBox(50, i * 40, 150, 30, self.pannel))
                    
                    self.labels.append(self.ui.text(str(i + 1), 10, i * 40, 30, 30, self.pannel))
            else:
                for i in range(newProps):#again necessary because properties pannel is updated
                    #so cannot simply remove the objects
                    self.textBoxes[i] = self.ui.textInputBox(50, i * 40, 150, 30, 
                        self.pannel, self.textBoxes[i].get_text())
                    
                    self.labels[i] = self.ui.text(str(i + 1), 10, i * 40, 30, 30, self.pannel)

                del self.textBoxes[newProps : oldProps]
                del self.labels[newProps : oldProps]

                                
                self.pannel.rebuild()
            
    def hide(self):
        for l in self.labels:
            l.hide()
            l.disable()
        for b in self.textBoxes:
            b.hide()
            b.disable()
        self.pannel.hide()
        self.pannel.disable()

    def show(self):
        for l in self.labels:
            l.show()
            l.enable()
        for b in self.textBoxes:
            b.show()
            b.enable()
        self.pannel.show()
        self.pannel.enable()

    def kill(self):
        for l in self.labels:
            l.kill()
        for b in self.textBoxes:
            b.kill()
        self.pannel.kill()

class Cell:
    def __init__(self):
        self.name = ""
        self.properties = []

        #only used by graphtissue:
        self.position = []
        self.radius = 0
        self.propertiesPanel = None
        self.gapJunctions = []
        self.connections = []

#scrollable field of cells that can be used to drag and drop cells 
#to tissue
class CellsButtons:
    def __init__(self, rect, ui):
        self.rect = pygame.Rect(*rect)
        self.ui = ui
        self.pannel = self.ui.scrollablePannel(0, 0, 0, 0, 0, 0)
        self.buttons = []
        self.cells = []
        
    def loadFromFile(self):
         cellfiles = glob.glob("working_files/*.cell")
         self.pannel.kill()
         self.pannel = self.ui.scrollablePannel(self.rect.x, self.rect.y, self.rect.width, self.rect.height, 
                                                            self.rect.width, len(cellfiles) * 33)
         
         self.buttons = []
         self.cells = []
         for i, filename in enumerate(cellfiles):
            with open(filename, 'r') as file: 
                 cell = Cell()
                 cell.name = file.readline().strip()
                 nprops = int(file.readline().strip())
                 for _ in range(nprops):
                    cell.properties.append(float(file.readline().strip()))
                 self.cells.append(cell)          
                 self.buttons.append(self.ui.button(cell.name, 10, i * 33, 200, 30, self.pannel))  

         self.pannel.rebuild()
            
    def hide(self):
        self.pannel.hide()
        self.pannel.disable()

    def show(self):
        self.pannel.show()
        self.pannel.enable()

class PackedTissueUIPannel:
    def __init__(self, rect,  window, ui):
        self.rect = pygame.Rect(*rect)
        self.window = window
        self.ui = ui
        self.tissueShape = [3, 3, 3]
        self.cellsButtons = CellsButtons((rect[0], 140, rect[2], rect[3] - 450), ui)
        self.cellsButtons.loadFromFile()
        self.propertiesInputs = [CellPropertyInputFields((rect[0], 310, rect[2], 200), ui) for i in range(27)]
        self.activePropertyInput = self.propertiesInputs[0]
        self.activePropertiesInputIndex = 0
        self.dragDropPayload = None
        self.cells = [Cell() for i in range(27)]
        self.curPath = os.path.dirname(os.path.abspath(__file__))
        self.options = ["Layer 1", "Layer 2", "Layer 3"]
        self.layer = 0
        self.fileDialog = None
        self.elements = self.draw()
        self.hide()

    def drawBackground(self):
        pygame.draw.rect(self.window.display, (40, 40, 40),
                         (self.rect.x, self.rect.y, self.rect.w, self.rect.height))
        
    def updateActivePropertiesInput(self, xy):
        if xy is not None and self.window.mousePressed[LEFT_MOUSE]:
            self.activePropertiesInputIndex = self.calcIndex(xy[0], xy[1], self.layer)
            self.activePropertyInput = self.propertiesInputs[self.activePropertiesInputIndex]
            for p in self.propertiesInputs:
                if p == self.activePropertyInput:
                    p.show()
                else:
                    p.hide()

    def draw(self):
        x, y, z = self.drawShapeInputField()
        return {
                "nameLabel" : self.ui.text("Tissue name", 15, 10, 90, 20),
                "nameTextBox" : self.ui.textInputBox(110, 5, 130, 30, value=''),
                "xLable" : x[0], "xTextBox" : x[1],
                "yLable" : y[0], "yTextBox" : y[1],
                "zLable" : z[0], "zTextBox" : z[1],
                "layerDropdown" : self.ui.dropDownList(10, 90, 200, 30, self.options),
                "cellsLabel" : self.ui.text("Cells", 10, 120, 50, 20),
                "propertiesLabel" : self.ui.text("Properties", 10, 288, 80, 20),
                "applyButton" : self.ui.button("Apply", 5, self.rect.height - 90, 80, 40),
                "saveButton" : self.ui.button("Save", 85, self.rect.height - 90, 80, 40),
                "loadButton" : self.ui.button("Load", 165, self.rect.height - 90, 80, 40),
                "generateButton" : self.ui.button("Generate", 5, self.rect.height - 50, 90, 40),
                "refreshButton" : self.ui.button("Refresh Cells", 95, self.rect.height - 50, 130, 40)
            }

    def onApply(self):
        if self.elements["applyButton"].check_pressed():
            x = int(self.elements["xTextBox"].get_text())
            y = int(self.elements["yTextBox"].get_text())
            z = int(self.elements["zTextBox"].get_text())
            if x < 1 or y < 1 or z < 1:
                self.ui.popup("Tissue must have at least one cell", "Save Failed", "Ok")
            old = self.tissueShape[0] * self.tissueShape[1] * self.tissueShape[2]
            new = x * y * z
            if new > old:
                for i in range(new - old):
                    self.cells.append(Cell())
                    self.propertiesInputs.append(CellPropertyInputFields(
                        (self.rect[0], 310, self.rect[2], 200), self.ui))
            else:
                self.cells = self.cells[:new]
                self.propertiesInputs = self.propertiesInputs[:new]

            self.tissueShape[0] = x
            self.tissueShape[1] = y
            self.tissueShape[2] = z
            self.options = ["Layer " + str(i + 1) for i in range(z)]
            self.elements["layerDropdown"].kill()
            self.elements["layerDropdown"] = self.ui.dropDownList(10, 90, 200, 30, self.options)
        selected = self.elements["layerDropdown"].selected_option
        self.layer = self.options.index(selected)

    def onGenerate(self):
        if self.elements["generateButton"].check_pressed():
            generatePackedTissuseHpp(self)

    def onRefresh(self):
        if self.elements["refreshButton"].check_pressed():
            self.cellsButtons.loadFromFile()

    def onSave(self):
        if self.elements["saveButton"].check_pressed():
             curPath = os.path.dirname(os.path.abspath(__file__))
             name = self.elements["nameTextBox"].get_text()

             if not name:
                self.ui.popup("Cell name cannot be empty", "Save Failed", "Ok")
                return

             if not os.path.exists(curPath + "\\working_files\\"):
                 os.makedirs(curPath + "\\working_files\\")

             for panel in self.propertiesInputs:
                for box in panel.textBoxes:
                    if not utils.isArithmatic(box.get_text()):
                        self.ui.popup("All properties must be a number", "Save Failed", "Ok")
                        return

             for x in range(self.tissueShape[0]):
                 for y in range(self.tissueShape[1]):
                     for z in range(self.tissueShape[2]):
                        cell = self.cells[self.calcIndex(x, y, z)]
                        if not cell.name:
                            self.ui.popup(
                            "Save Failed", f"All cells must be set before saving. Unset cell: x = {x + 1}, y = {y + 1}, layer = {z + 1}", "Ok")
                            return

             with open(curPath + "\\working_files\\" + name + ".ptsue", "w") as file:
                 file.write(name + '\n')
                 for i in range(3):
                    file.write(str(self.tissueShape[i]) + '\n')
                 
                 for i, cell in enumerate(self.cells):
                     file.write(cell.name + '\n')
                     file.write(str(len(cell.properties)) + '\n')
                     for j in range(len(cell.properties)):
                         file.write(self.propertiesInputs[i].textBoxes[j].get_text() + '\n')

             self.ui.popup("Save successful", "Save Success", "Ok")

    def onLoad(self):
         path = ""
         if self.elements["loadButton"].check_pressed():
             self.fileDialog = self.ui.fileDialog("Load Packed Tissue", 300, 100, 500, 500, "ptsue")
             
         out = False
         if self.fileDialog is not None:
             for event in self.window.events:
                 if event.type == pygame.USEREVENT:
                     if event.ui_element == self.fileDialog.ok_button:
                         if self.fileDialog.ok_button.check_pressed():
                            out = True
                            path = self.fileDialog.current_file_path
                            self.fileDialog = None
                            self.layer = 0
                            with open(path, "r") as file:
                                self.elements["nameTextBox"].set_text(file.readline().strip())
                                self.tissueShape[0] = int(file.readline().strip())
                                self.tissueShape[1] = int(file.readline().strip())
                                self.tissueShape[2] = int(file.readline().strip())
                                self.elements["xTextBox"].set_text(str(self.tissueShape[0]))
                                self.elements["yTextBox"].set_text(str(self.tissueShape[1]))
                                self.elements["zTextBox"].set_text(str(self.tissueShape[2]))
                                self.options = ["Layer " + str(i + 1) for i in range(self.tissueShape[2])]
                                self.elements["layerDropdown"].kill()
                                self.elements["layerDropdown"] = self.ui.dropDownList(10, 90, 200, 30, self.options)
                                ncells = self.tissueShape[0] * self.tissueShape[1] * self.tissueShape[2]
                                self.cells = [Cell() for i in range(ncells)]
                                for p in self.propertiesInputs:
                                    p.kill()
                                self.propertiesInputs = [CellPropertyInputFields((0, 0, 0, 0), self.ui) for i in range(ncells)]
                                for i in range(ncells):
                                    self.cells[i].name = file.readline().strip()
                                    nprops = int(file.readline().strip())
                                    self.propertiesInputs[i].pannel.kill()
                                    self.propertiesInputs[i].pannel = self.ui.scrollablePannel(
                                                    self.rect[0], 310, self.rect[2], 200,
                                                    self.rect.width - 20, nprops * 40)
                                    for j in range(nprops):
                                        propValue = float(file.readline().strip())
                                        self.cells[i].properties.append(propValue)

                                        self.propertiesInputs[i].textBoxes.append(self.ui.textInputBox(50, j * 40, 150, 30, 
                                            self.propertiesInputs[i].pannel, str(propValue)))
                                        
                                        self.propertiesInputs[i].labels.append(self.ui.text(str(j + 1), 10, j * 40, 30, 30, 
                                                                                    self.propertiesInputs[i].pannel))
                                    self.propertiesInputs[i].pannel.rebuild()
                                    self.propertiesInputs[i].hide()
                                                                            
                            break
                         
         return out

    def handleCellDragDrop(self, xy):
        index = 0
        if self.dragDropPayload is None:
            for i, button in enumerate(self.cellsButtons.buttons):
                for event in self.window.events:
                    if event.type == pygame.MOUSEBUTTONDOWN:
                        if button.rect.collidepoint(event.pos):
                            index = i
                            self.dragDropPayload = self.cellsButtons.cells[i]#picked up payload

        if self.dragDropPayload is not None and xy is not None:
            x, y = xy
            index = self.calcIndex(x, y, self.layer)             
            if self.window.mouseReleased[LEFT_MOUSE]:
                self.cells[index] = self.dragDropPayload 
                nprops = len(self.dragDropPayload.properties)
                self.propertiesInputs[index].apply(nprops)    
                for i in range(nprops):
                     self.propertiesInputs[index].textBoxes[i].set_text(
                         str(self.dragDropPayload.properties[i])
                     )
                for i, p in enumerate(self.propertiesInputs):
                    if i == index:
                        p.show()
                    else:
                        p.hide()
                self.dragDropPayload = None#payload delivered
                return True

        elif self.dragDropPayload is not None and not self.window.mouseHeld[LEFT_MOUSE]:
            self.dragDropPayload = None#invalid delivery location

        mouse = pygame.mouse.get_pos()
        if self.dragDropPayload is not None:
            font = pygame.freetype.Font(self.curPath + "\\Assets\\Font\\open-sans\\OpenSans-Regular.ttf", 
                                        25)
            font.render_to(self.window.display, (mouse[0] - len(self.dragDropPayload.name) * 5, mouse[1] - 15), 
                           self.dragDropPayload.name, (180, 200, 180, 230))               
        return False

    def drawShapeInputField(self):
        return (
            (self.ui.text("x", 10, 50, 20, 20), 
            self.ui.intTextInputBox(25, 40, 40, 40, value='3')),
            (self.ui.text("y", 80, 50, 20, 20), 
            self.ui.intTextInputBox(95, 40, 40, 40, value='3')),
            (self.ui.text("z", 150, 50, 20, 20), 
            self.ui.intTextInputBox(165, 40, 40, 40, value='3'))
        )
    
    def hide(self):
        self.cellsButtons.hide()
        for _, value in self.elements.items():
            value.hide()
            value.disable()
        for p in self.propertiesInputs:
            p.hide()

    def show(self):
        self.cellsButtons.show()
        for  _, value in self.elements.items():
            value.show()
            value.enable()
        self.activePropertyInput.show()

    def calcIndex(self, x, y, z):
        return (z * self.tissueShape[0] * self.tissueShape[1]) + y * self.tissueShape[0] + x
                
class CellUIPannel:
    def __init__(self, rect,  window, ui):
        self.rect = pygame.Rect(*rect)
        self.window = window
        self.ui = ui
        self.elements = self.draw()
        self.propertiesInput = CellPropertyInputFields((rect[0], 160, rect[2], rect[3] - 260), ui)
        self.fileDialog = None
        self.hide()

    def drawBackground(self):
        pygame.draw.rect(self.window.display, (40, 40, 40),
                         (self.rect.x, self.rect.y, self.rect.w, self.rect.height))
        
    def draw(self):
        nameLabel, nameTextBox = self.drawNameInputField()
        nPropsLabel, nPropsTextBox = self.drawNumPropsInputField()
        return {
              "nPropsLabel" : nPropsLabel,
              "nPropsTextBox" : nPropsTextBox,
              "nameLabel" : nameLabel,
              "nameTextBox" : nameTextBox,
              "applyButton" : self.ui.button("Apply", 5, self.rect.height - 90, 80, 80),
              "saveButton" : self.ui.button("Save", 85, self.rect.height - 90, 80, 80),
              "loadButton" : self.ui.button("Load", 165, self.rect.height - 90, 80, 80),
              "propertiesLabel" : self.ui.text("Properties", 5, 130, 100, 30)
            }
    
    def drawNameInputField(self):
        return  (self.ui.text("Cell name", 15, 35, 77, 20), 
                 self.ui.textInputBox(100, 25, 130, 40, value=''))

    def drawNumPropsInputField(self):
        return  (self.ui.text("Number of properties", 15, 85, 160, 20), 
                 self.ui.intTextInputBox(178, 78, 40, 40))
    
    def onApply(self):
        if self.elements["applyButton"].check_pressed():
            self.name = self.elements["nameTextBox"]
            if (self.elements["nPropsTextBox"].get_text() != ""):
                newProps = int(self.elements["nPropsTextBox"].get_text())
                self.propertiesInput.apply(newProps)

    def onSave(self):
        if self.elements["saveButton"].check_pressed():
            name = self.elements["nameTextBox"].get_text() 
            if not name:
                self.ui.popup("Cell name cannot be empty", "Save Failed", "Ok")
                return
            curPath = os.path.dirname(os.path.abspath(__file__))
            if not len(self.propertiesInput.textBoxes):
                self.ui.popup("Cannot save cell with 0 properties", "Save Failed", "Ok")
                return
            if not os.path.exists(curPath + "\\working_files\\"):
                 os.makedirs(curPath + "\\working_files\\")

            for box in self.propertiesInput.textBoxes:
                if not utils.isArithmatic(box.get_text()):
                    self.ui.popup("All properties must be a number", "Save Failed", "Ok")
                    return

            with open(curPath + "\\working_files\\" + name + ".cell", "w") as file:
                file.write(name + '\n')
                file.write(str(len(self.propertiesInput.textBoxes)) + '\n')
                for box in self.propertiesInput.textBoxes:                
                    file.write(box.get_text() + '\n')

            self.ui.popup("Save successful", "Save Success", "Ok")

    def onLoad(self):
         path = ""
         if self.elements["loadButton"].check_pressed():
             self.fileDialog = self.ui.fileDialog("Load Cell", 300, 100, 500, 500, "cell")
             
         if self.fileDialog is not None:
             for event in self.window.events:
                 if event.type == pygame.USEREVENT:
                     if event.ui_element == self.fileDialog.ok_button:
                         if self.fileDialog.ok_button.check_pressed():
                            path = self.fileDialog.current_file_path
                            self.fileDialog = None
                            with open(path, "r") as file:
                                self.elements["nameTextBox"].set_text(file.readline().strip())
                                nprops = file.readline().strip()
                                self.elements["nPropsTextBox"].set_text(nprops)
                                self.propertiesInput.loadFromFile(file, int(nprops))
                            break    

    def hide(self):
        self.propertiesInput.hide()
        for _, value in self.elements.items():
            value.hide()
            value.disable()

    def show(self):
        self.propertiesInput.show()
        for  _, value in self.elements.items():
            value.show()
            value.enable()
    
class GraphTissueUIPannel:
    def __init__(self, rect,  window, ui):
        self.rect = pygame.Rect(*rect)
        self.window = window
        self.ui = ui
        self.cellsButtons = CellsButtons((rect[0], 60, rect[2], rect[3] - 450), ui)
        self.cellsButtons.loadFromFile()
        self.propertiesInputs = []
        self.cells = []
        self.dragDropPayload = None
        self.curPath = os.path.dirname(os.path.abspath(__file__))
        self.elements = self.draw()
        self.currentCell = None
        self.connectionFrom = None
        self.connectionFromPos = None
        self.currentConnection = None
        self.fileDialog = None
        self.hide()

    def handleCellDragDrop(self, xy):
        if self.dragDropPayload is None:
            for i, button in enumerate(self.cellsButtons.buttons):
                for event in self.window.events:
                    if event.type == pygame.MOUSEBUTTONDOWN:
                        if button.rect.collidepoint(event.pos):
                            self.dragDropPayload = self.cellsButtons.cells[i]#picked up payload

        if self.dragDropPayload is not None:
            if self.window.mouseReleased[LEFT_MOUSE]:
                mpos = pygame.mouse.get_pos()
                if mpos[0] > self.rect.width and mpos[1] > 50:
                    cell = copy.deepcopy(self.dragDropPayload)
                    cell.position = xy
                    self.cells.append(cell) 
                    nprops = len(cell.properties)
                    self.propertiesInputs.append(CellPropertyInputFields(
                        (self.rect[0], 240, self.rect[2], 200), self.ui))
                    self.propertiesInputs[-1].apply(nprops)    
                    for i in range(nprops):
                        self.propertiesInputs[-1].textBoxes[i].set_text(
                            str(self.dragDropPayload.properties[i])
                        )
                    for i in range(len(self.propertiesInputs) - 1):
                        self.propertiesInputs[i].hide()
                    self.propertiesInputs[-1].show()
                    self.cells[-1].propertiesPanel = self.propertiesInputs[-1]
                    self.dragDropPayload = None#payload delivered
                else:
                    self.dragDropPayload = None

        elif self.dragDropPayload is not None and not self.window.mouseHeld[LEFT_MOUSE]:
            self.dragDropPayload = None#invalid delivery location

        mouse = pygame.mouse.get_pos()
        if self.dragDropPayload is not None:
            font = pygame.freetype.Font(self.curPath + "\\Assets\\Font\\open-sans\\OpenSans-Regular.ttf", 
                                        25)
            font.render_to(self.window.display, (mouse[0] - len(self.dragDropPayload.name) * 5, mouse[1] - 15), 
                           self.dragDropPayload.name, (180, 200, 180, 230))

    def draw(self):
        return {
                "nameLabel" : self.ui.text("Tissue name", 15, 10, 90, 20),
                "nameTextBox" : self.ui.textInputBox(110, 5, 130, 30, value=''),
                "cellsLabel" : self.ui.text("Cells", 10, 40, 50, 20),
                "propertiesLabel" : self.ui.text("Properties", 10, 210, 80, 20),
                "gapJunctionLabel" : self.ui.text("Gap Junctions", 10, 445, 105, 20),
                "saveButton" : self.ui.button("Save", 5, 480, 80, 45),
                "loadButton" : self.ui.button("Load", 95, 480, 80, 45),
                "generateButton" : self.ui.button("Generate", 5, 530, 90, 45),
                "refreshButton" : self.ui.button("Refresh Cells", 100, 530, 140, 45)
            }

    def handleAddingConnections(self, cell):
        mpos = pygame.mouse.get_pos()
        if self.window.keyHeld[pygame.K_w] and cell is not None:
            if self.window.mouseHeld[LEFT_MOUSE] and self.connectionFrom is None:
                self.connectionFrom = cell
                self.connectionFromPos = mpos
            if self.window.mouseReleased[LEFT_MOUSE] and self.connectionFrom is not None:
                if cell in self.connectionFrom.connections or self.connectionFrom in cell.connections or cell == self.connectionFrom:
                    self.ui.popup("Couldn't add connection", 
                        "Couldnt add connection since the connection already exists or it is a connection to itself", "Ok")
                    self.connectionFrom = None
                    self.connectionFromPos = None
                    return 
                self.connectionFrom.connections.append(cell)
                self.connectionFrom.gapJunctions.append(
                    self.ui.intTextInputBox(130, 440, 105, 30, value='1'))
                self.connectionFrom.gapJunctions[-1].disable()
                self.connectionFrom.gapJunctions[-1].hide()
                self.connectionFrom = None
                self.connectionFromPos = None
        if self.window.mouseReleased[LEFT_MOUSE] and self.connectionFrom is not None:
                self.connectionFrom = None
                self.connectionFromPos = None
        if self.connectionFromPos is not None:
            pygame.draw.aaline(self.window.display, (255, 255, 255, 100), self.connectionFromPos, mpos)

    def onSave(self):
        if self.elements["saveButton"].check_pressed():
             curPath = os.path.dirname(os.path.abspath(__file__))
             name = self.elements["nameTextBox"].get_text()

             if not name:
                self.ui.popup("Cell name cannot be empty", "Save Failed", "Ok")
                return

             if not os.path.exists(curPath + "\\working_files\\"):
                 os.makedirs(curPath + "\\working_files\\")

             for cell in self.cells:
                for box in cell.propertiesPanel.textBoxes:
                    if not utils.isArithmatic(box.get_text()):
                        self.ui.popup("Save Failed", "All properties must be a number", "Ok")
                        return
                                        
             with open(curPath + "\\working_files\\" + name + ".gtsue", "w") as file:
                 file.write(self.elements["nameTextBox"].get_text() + '\n')
                 file.write(str(len(self.cells)) + '\n')
                 for i, cell in enumerate(self.cells):
                     file.write(cell.name + '\n')
                     file.write(str(cell.position[0]) + ", " + str(cell.position[1]) + '\n')
                     file.write(str(cell.radius) + '\n')
                     file.write(str(len(cell.properties)) + '\n')
                     for j in range(len(cell.properties)):
                         file.write(self.propertiesInputs[i].textBoxes[j].get_text() + '\n')
                 
                 
                 for cell in self.cells:
                     for i in range(len(cell.connections)):
                         index = self.cells.index(cell.connections[i])
                         if i == len(cell.connections) - 1:
                            file.write(str(index))
                         else:
                            file.write(str(index) + ", ")
                     if cell.connections:
                        file.write('\n')
                     else:
                         file.write(str(-1) + '\n')
                     for i in range(len(cell.gapJunctions)):
                         if i == len(cell.connections) - 1:
                            file.write(cell.gapJunctions[i].get_text())
                         else:
                            file.write(cell.gapJunctions[i].get_text() + ", ")
                     if cell.gapJunctions:
                        file.write('\n')
                     else:
                         file.write(str(-1) + '\n')

             self.ui.popup("Save successful", "Save Success", "Ok")

    def onGenerate(self):
        if self.elements["generateButton"].check_pressed():
            generateGraphTissueHpp(self)

    def onLoad(self):
         path = ""
         if self.elements["loadButton"].check_pressed():
             self.fileDialog = self.ui.fileDialog("Load Graph Tissue", 300, 100, 500, 500, "gtsue")
             
         out = False
         if self.fileDialog is not None:
             for event in self.window.events:
                 if event.type == pygame.USEREVENT:
                     if event.ui_element == self.fileDialog.ok_button:
                         if self.fileDialog.ok_button.check_pressed():
                            out = True
                            path = self.fileDialog.current_file_path
                            self.fileDialog = None
                            with open(path, "r") as file:
                                self.elements["nameTextBox"].set_text(file.readline().strip())
                                ncells = int(file.readline().strip())
                                for cell in self.cells:
                                    for g in cell.gapJunctions:
                                        g.kill()
                                self.cells = [Cell() for i in range(ncells)]
                                for p in self.propertiesInputs:
                                    p.kill()
                                self.propertiesInputs = [CellPropertyInputFields((0, 0, 0, 0), self.ui) for i in range(ncells)]
                                for i in range(ncells):
                                    self.cells[i].name = file.readline().strip()
                                    self.cells[i].position = file.readline().strip().split(", ")
                                    self.cells[i].position[0] = float(self.cells[i].position[0])
                                    self.cells[i].position[1] = float(self.cells[i].position[1])
                                    self.cells[i].radius = float(file.readline().strip())
                                    nprops = int(file.readline().strip())
                                    self.propertiesInputs[i].pannel.kill()
                                    self.propertiesInputs[i].pannel = self.ui.scrollablePannel(
                                                     self.rect[0], 240, self.rect[2], 200,
                                                    self.rect.width - 20, nprops * 40)
                                    self.cells[i].propertiesPanel = self.propertiesInputs[i]
                                    for j in range(nprops):
                                        propValue = float(file.readline().strip())
                                        self.cells[i].properties.append(propValue)

                                        self.propertiesInputs[i].textBoxes.append(self.ui.textInputBox(50, j * 40, 150, 30, 
                                            self.propertiesInputs[i].pannel, str(propValue)))
                                        
                                        self.propertiesInputs[i].labels.append(self.ui.text(str(j + 1), 10, j * 40, 30, 30, 
                                                                                     self.propertiesInputs[i].pannel))
                                    self.propertiesInputs[i].pannel.rebuild()
                                    self.propertiesInputs[i].hide()

                                for cell in self.cells:
                                    cons = file.readline().strip().split(", ")
                                    for con in cons:
                                        if con != "-1":
                                            cell.connections.append(self.cells[int(con)])
                                    gaps = file.readline().strip().split(", ")
                                    for gap in gaps:
                                        if gap != "-1":
                                            cell.gapJunctions.append(self.ui.intTextInputBox(130, 440, 105, 30, value=gap)) 
                                            cell.gapJunctions[-1].rebuild()
                                    for g in cell.gapJunctions:
                                        g.hide()
                                        g.disable()
                                    
                            break   
         return out

    def onRefresh(self):
        if self.elements["refreshButton"].check_pressed():
            self.cellsButtons.loadFromFile()

    def hide(self):
        self.cellsButtons.hide()
        for cell in self.cells:
            cell.propertiesPanel.hide()
            for g in cell.gapJunctions:
                g.hide()
                g.disable()            
        for _, value in self.elements.items():
            value.hide()
            value.disable()

    def show(self):
        self.cellsButtons.show()
        if self.currentCell is not None:
            self.currentCell.propertiesPanel.show()
            for g in self.currentCell.gapJunctions:
                g.show()
                g.enable()
        for  _, value in self.elements.items():
            value.show()
            value.enable()

    def drawBackground(self):
        pygame.draw.rect(self.window.display, (40, 40, 40),
                         (self.rect.x, self.rect.y, self.rect.w, self.rect.height))
