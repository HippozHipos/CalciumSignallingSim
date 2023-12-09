#pragma once

namespace csim {

    struct Probabilities
    {
        static float phl[50];

        // phl - probability of having low and high conducate - close - open state = closed gap junction
        static float plh[50];

        // phl - probability of having high and high conducate - open - open state = open gap junction
        static float phh[50];
    };

}