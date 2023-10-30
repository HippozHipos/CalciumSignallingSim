#pragma once

#include "TissueCellTypeList.hpp"
#include "Cell.hpp"

namespace generated {

    template<class T>
    using GraphTissueCellTypes = 
    TissueCellTypeList
    <     
        SmoothMuscle<T>,
        Astrocyte<T>,
        Astrocyte<T>,
        Astrocyte<T>,
        SmoothMuscle<T>
    >;

    template<class T>
    std::initializer_list<std::initializer_list<T>> GraphTissueCellValues = 
    {
        {
            6.0, 7.0, 8.0, 9.0, 10.0
        },

        {
            1.0, 2.0, 3.0, 4.0, 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.001, 0.0, 55.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 12.0
        },

        {
            0.001, 0.002, 0.003, 0.004, 0.005, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.001, 0.0, 55.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 12.0
        },

        {
            12.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.001, 0.0, 55.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 12.0
        },

        {
            0.0, 0.0, 0.0, 0.0, 0.0
        }
    };

    std::initializer_list<std::initializer_list<size_t>> GraphTissueConnections = 
    {
        {
            2
        },

        {
            0
        },

        {
            3
        },

        {
            
        }, 

        {
            1
        }
    };

    std::initializer_list<std::initializer_list<size_t>> GraphTissueGapJunctions = 
    {
        {
            3
        },

        {
            2
        },

        {
            1
        },

        {
            
        }, 

        {
            1
        }
    };

}