// function.cpp
// Copyright (C) 2023 Alexander Karpeko

#include "function.h"

void limit(int &value, int min, int max)
{
    if (value < min)
        value = min;
    if (value > max)
        value = max;
}
