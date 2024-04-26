import os

def makePackedTissuseContents(tissue):
    packedTissueHppContents = """#pragma once

#include \"TissueCellTypeList.hpp\"
#include "Cell.hpp\"
#include <array>
#include <initializer_list>

namespace generated {

    template<class T>
    using PackedTissueCellTypes = 
    TissueCellTypeList
    <     
"""
    for i, cell in enumerate(tissue.cells):
        end = "<T>\n" if i == len(tissue.cells) - 1 else "<T>,\n"
        packedTissueHppContents += "        " + cell.name + end
    packedTissueHppContents += "    >;\n\n"

    packedTissueHppContents += "    template<class T>\n"
    packedTissueHppContents += "    std::initializer_list<std::initializer_list<T>> PackedTissueCellValues = \n    {"

    for i, cell in enumerate(tissue.cells):
        packedTissueHppContents += "\n        {\n            "
        for j in range(len(cell.properties)):
            end = "\n        },\n" if j == len(cell.properties) - 1 else ", "
            if i == len(tissue.cells) - 1 and j == len(cell.properties) - 1:
                end =  "\n        }\n"
            packedTissueHppContents += tissue.propertiesInputs[i].textBoxes[j].get_text() + end
    packedTissueHppContents += "    };\n\n"
    packedTissueHppContents += "    std::array<size_t, 3> PackedTissueShape = { " + tissue.elements["xTextBox"].get_text() + ", " + tissue.elements["xTextBox"].get_text() + ", "  + tissue.elements["xTextBox"].get_text() + " };\n}"
    

    return packedTissueHppContents

def generatePackedTissuseHpp(tissue):
    contents = makePackedTissuseContents(tissue)
    file = os.path.dirname(os.path.abspath(__file__)) + "\\..\\"
    with open(file + "GeneratedPackedTissueInfo.hpp", 'w') as file:
        file.write(contents)
    tissue.ui.popup("Success", "GeneratedPackedTissueInfo.hpp file has been generated in:\n" + file.name, "Ok")


def makeGraphTissueContents(tissue):
    graphTissueHppContents = """#pragma once

#include \"TissueCellTypeList.hpp\"
#include "Cell.hpp\"

namespace generated {

    template<class T>
    using GraphTissueCellTypes = 
    TissueCellTypeList
    <     
"""
    for i, cell in enumerate(tissue.cells):
        end = "<T>\n" if i == len(tissue.cells) - 1 else "<T>,\n"
        graphTissueHppContents += "        " + cell.name + end
    graphTissueHppContents += "    >;\n\n"

    graphTissueHppContents += "    template<class T>\n"
    graphTissueHppContents += "    std::initializer_list<std::initializer_list<T>> GraphTissueCellValues = \n    {"
    for i, cell in enumerate(tissue.cells):
        graphTissueHppContents += "\n        {\n            "
        for j in range(len(cell.properties)):
            end = "\n        },\n" if j == len(cell.properties) - 1 else ", "
            if i == len(tissue.cells) - 1 and j == len(cell.properties) - 1:
                end =  "\n        }\n"
            graphTissueHppContents += tissue.propertiesInputs[i].textBoxes[j].get_text() + end
    graphTissueHppContents += "    };\n\n"

    graphTissueHppContents += "    std::initializer_list<std::initializer_list<size_t>> GraphTissueConnections = \n    {"
    for i, cell in enumerate(tissue.cells):
        graphTissueHppContents += "\n        {\n            "
        for j in range(len(cell.connections)):
            end = "\n        },\n" if j == len(cell.connections) - 1 else ", "
            if i == len(tissue.cells) - 1 and j == len(cell.connections) - 1:
                end =  "\n        }\n"
            graphTissueHppContents += str(tissue.cells.index(cell.connections[j])) + end
        if not cell.connections:
            end = "" if i == len(tissue.cells) - 1 else ", "
            graphTissueHppContents += "\n        }" + end + '\n'
    graphTissueHppContents += "    };\n\n"

    graphTissueHppContents += "    std::initializer_list<std::initializer_list<size_t>> GraphTissueGapJunctions = \n    {"
    for i, cell in enumerate(tissue.cells):
        graphTissueHppContents += "\n        {\n            "
        for j in range(len(cell.gapJunctions)):
            end = "\n        },\n" if j == len(cell.gapJunctions) - 1 else ", "
            if i == len(tissue.cells) - 1 and j == len(cell.gapJunctions) - 1:
                end =  "\n        }\n"
            graphTissueHppContents += str(cell.gapJunctions[j].get_text()) + end
        if not cell.gapJunctions:
            end = "" if i == len(tissue.cells) - 1 else ", "
            graphTissueHppContents += "\n        }" + end + '\n'
    graphTissueHppContents += "    };\n\n"

    graphTissueHppContents += '}'

    return graphTissueHppContents

def generateGraphTissueHpp(tissue):
    contents = makeGraphTissueContents(tissue)
    file = os.path.dirname(os.path.abspath(__file__)) + "\\..\\"
    with open(file + "GeneratedGraphTissueInfo.hpp", 'w') as file:
        file.write(contents)
    tissue.ui.popup("Success", "GeneratedGraphTissueInfo.hpp file has been generated in:\n" + file.name, "Ok")