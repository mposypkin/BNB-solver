#include "rnagen.hpp"

double DNA_TRANSITION_MATRIX[4][4] = 
{
{0.030, 0.061, 0.105, 0.049},
{0.029, 0.041, 0.100, 0.042},
{0.086, 0.076, 0.158, 0.085},
{0.089, 0.110, 0.137, 0.084}
};
    
double DNA_OXIDATION_VECTOR[4] = {0.45, 0.66, 0.66, 0.};

double RnaGenerator::mScale = 1.;
