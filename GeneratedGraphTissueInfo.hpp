#pragma once

#include "TissueCellTypeList.hpp"
#include "Cell.hpp"

namespace generated {

    template<class T>
    using GraphTissueCellTypes = 
    TissueCellTypeList
    <     
        Astrocyte<T>,
        SmoothMuscle<T>,
        Astrocyte<T>,
        SmoothMuscle<T>
    >;

    template<class T>
    std::initializer_list<std::initializer_list<T>> GraphTissueCellValues = 
    {
        {
            5.0, 10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
        },

        {
            1.0, 2.0, 0.0, 0.0, 0.0
        },

        {
            5.0, 10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
        },

        {
            1.0, 2.0, 0.0, 0.0, 0.0
        }
    };

    std::initializer_list<std::initializer_list<size_t>> GraphTissueConnections = 
    {
        {
            3
        },

        {
            0
        },

        {
            0, 3
        },

        {
            1
        }
    };

    std::initializer_list<std::initializer_list<size_t>> GraphTissueGapJunctions = 
    {
        {
            4
        },

        {
            1
        },

        {
            2, 1
        },

        {
            2
        }
    };

}