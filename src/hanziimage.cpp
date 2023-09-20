// hanziimage.cpp
// Copyright (C) 2023 Alexander Karpeko

#include "hanziimage.h"
#include <cmath>
#include <cstring>
#include <iostream>

ImageBorder findImageBorder(uint64_t *image, int side)
{
    bool isImage = false;
    ImageBorder imageBorder = {0, 0, 0, 0, 0, 0};

    bool setValue = false;
    for (int y = 0; y < side; y++)
        for (int x = 0; x < side; x++)
            if (!setValue && image[side*y+x] != 0) {
                isImage = true;
                setValue = true;
            }

    setValue = false;
    for (int y = 0; y < side; y++)
        for (int x = 0; x < side; x++)
            if (!setValue && image[side*y+x] != 0) {
                imageBorder.top = y;
                setValue = true;
            }

    setValue = false;
    for (int y = side - 1; y >= 0; y--)
        for (int x = 0; x < side; x++)
            if (!setValue && image[side*y+x] != 0) {
                imageBorder.bottom = y;
                setValue = true;
            }

    setValue = false;
    for (int x = 0; x < side; x++)
        for (int y = 0; y < side; y++)
            if (!setValue && image[side*y+x] != 0) {
                imageBorder.left = x;
                setValue = true;
            }

    setValue = false;
    for (int x = side - 1; x >= 0; x--)
        for (int y = 0; y < side; y++)
            if (!setValue && image[side*y+x] != 0) {
                imageBorder.right = x;
                setValue = true;
            }

    if (isImage) {
        imageBorder.width = imageBorder.right - imageBorder.left + 1;
        imageBorder.height = imageBorder.bottom - imageBorder.top + 1;
    }

    return imageBorder;
}

/*
 * Image cell: uint64_t
 * High 28 bits, 1 bit, 1 bit, 1 bit, 1 bit,
 * 12 bits, 8 bits, 5 bits, 6 bits, 1 bit Low
 * 28 bits: reserved
 * 1 bit: output wave flag
 * 1 bit: line cell
 * 1 bit: local maximum cell
 * 1 bit: start cell
 * 12 bits: wave value
 * 8 bits: previous points
 * 5 bits: line group number = 1 ... 31
 * 6 bits: space number (global) = 1 ... 63
 *         or line number in group = 1 ... 63
 * 1 bit: cell type, space: 0, symbol: 1
 */

void wavePropagation(uint64_t *image, int side, bool symbolCells,
                     uint64_t groupNumber, int startX, int startY)
{
    if (symbolCells && (groupNumber < minLineGroupNumber || groupNumber > maxLineGroupNumber)) {
        std::cerr << "Error: groupNumber is out of range for symbol cells\n";
        return;
    }

    if (!symbolCells && (groupNumber < minSpaceNumber || groupNumber > maxSpaceNumber)) {
        std::cerr << "Error: groupNumber is out of range for space cells\n";
        return;
    }

    if (startX < 0 || startX >= side) {
        std::cerr << "Error: startX is out of range 0 ... side - 1\n";
        return;
    }

    if (startY < 0 || startY >= side) {
        std::cerr << "Error: startY is out of range 0 ... side - 1\n";
        return;
    }

    uint64_t newCellNumber = newSymbolCellNumber;
    uint64_t offset = lineGroupNumberOffset;

    if (!symbolCells) {
        newCellNumber = newSpaceCellNumber;
        offset = spaceNumberOffset;
    }

    constexpr int maxWaveSize = 64 * 1024;
    int inputWaveSize = 1;
    int outputWaveSize = 0;
    int inputWave[maxWaveSize][2];   // x, y
    int outputWave[maxWaveSize][2];  // x, y

    image[side*startY+startX] |= groupNumber << offset;
    inputWave[0][0] = startX;
    inputWave[0][1] = startY;

    while (true) {
        outputWaveSize = 0;
        for (int i = 0; i < inputWaveSize; i++) {
            int x = inputWave[i][0];
            int y = inputWave[i][1];
            for (int dy = -1; dy < 2; dy++)
                for (int dx = -1; dx < 2; dx++) {
                    if (dx == 0 && dy == 0)
                        continue;
                    if (x + dx < 0 || x + dx >= side ||
                        y + dy < 0 || y + dy >= side)
                        continue;
                    if (image[side*(y+dy)+x+dx] != newCellNumber)
                        continue;
                    image[side*(y+dy)+x+dx] &= 0xfffff07f;
                    image[side*(y+dy)+x+dx] |= static_cast<uint64_t>(groupNumber << offset);
                    outputWave[outputWaveSize][0] = x + dx;
                    outputWave[outputWaveSize][1] = y + dy;
                    outputWaveSize++;
                }
        }
        if (outputWaveSize == 0)
            return;
        if (outputWaveSize > maxWaveSize) {
            std::cerr << "Error: outputWaveSize > maxWaveSize\n";
            std::cerr << "outputWaveSize = " << outputWaveSize <<
                         " maxWaveSize = " << maxWaveSize << std::endl;
            return;
        }
        for (int i = 0; i < outputWaveSize; i++) {
            inputWave[i][0] = outputWave[i][0];
            inputWave[i][1] = outputWave[i][1];
            inputWaveSize = outputWaveSize;
        }
    }
}
