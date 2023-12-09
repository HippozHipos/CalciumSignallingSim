#pragma once

#include <vector>
#include <assert.h>
#include "Cell.h"

//Tissie memory representation:
//Concept: 
//Tissue is made up of layers. layer is a 2d tissue. Stack layers to make 3d tissue.
//Each layer of tissue is a 1d-array representation of a 2d array.
//Each 3d tissue is a 1d-array representation of a bunch of 2d layers.
//Example: 3x3 layer (c is cell and index starts at 0)
//a conceptual tissue layer:
//c c c
//c c c
//c c c
//can be represented in memory as 
//c c c c c c c c c
//conversion from 2d layer to 1d memory = y * layerWidth + x
//concrete example: we want the the cell at x, y = 1, 2: 
// y * layerWidth + x = 2 * 3 + 1 = 7th cell
//Now 3x3x3 tissue would be 3 of thoose layers "stacked" on top of each other.
//represented in memory as 
//(layer1)c c c c c c c c c (layer2)c c c c c c c c c c (layer3)c c c c c c c c c
//we can go from 3d tissue to 2d layer with: 
//z * number of cells in a layer + (y * layerWidth + x):
//concrete example: we want the cell at x, y, z = 1, 2, 2:
//z * number of cells in a layer + (y * layerWidth + x) = 
//2 * 9 + (2 * 3 + 1) = 25th cell

namespace csim {

	template<size_t width, size_t height, size_t depth>
	class Tissue
	{
	public:
		Tissue()
		{
		}

	public:
		csim::Cell& GetCell(size_t x, size_t y, size_t z)
		{
			return m_Cells[Index(x, y, z)];
		}

		const csim::Cell& GetCellAt(size_t x, size_t y, size_t z) const
		{
			return m_Cells[Index(x, y, z)];
		}

		void SetCellAt(size_t x, size_t y, size_t z, const csim::Cell& cell)
		{
			m_Cells[Index(x, y, z)] = cell;
		}

		void ConstructFromSample(const csim::Cell& cell)
		{
			for (int i = 0; i < width * height * depth; i++)
				m_Cells[i] = cell;
		}

	public:
		size_t Width() const { return width; }
		size_t Height() const { return height; }
		size_t Depth() const { return depth; }
		size_t Size() const{ return Width() * Height() * Depth(); }

	private:
		size_t Index(size_t x, size_t y, size_t z) const
		{
			assert((x <= width - 1) && (y <= height - 1) && (x <= depth - 1));
			return z * (width * height) + (y * width + x);
		}

	private:
		Cell m_Cells[width * height * depth];
	};

}