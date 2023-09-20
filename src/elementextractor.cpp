// elementextractor.h
// Copyright (C) 2023 Alexander Karpeko

#include "elementextractor.h"
#include "hanziimage.h"
#include <algorithm>
#include <cstring>
#include <iostream>

ElementExtractor::ElementExtractor(const uint64_t *image, int side,
                                   ImageElements &imageElements):
    image(image), side(side), imageElements(imageElements)
{
    if (side < 1 || side > maxSide)
        throw "ElementExtractor side is out of range: 1 ... maxExtractorImageSide";

    maxWidth = side / 10;
    minLength = side / 6;
    memset(static_cast<void*>(&imageElements), 0, sizeof(ImageElements));
}

bool ElementExtractor::findElements()
{
    findLineGroups();
    findSpaces();

    return true;
}

void ElementExtractor::findLineGroups()
{
    std::copy(image, image + side * side, tmpImage);

    uint64_t groupNumber = minLineGroupNumber;
    for (int y = 0; y < side; y++)
        for (int x = 0; x < side; x++) {
            if (tmpImage[side*y+x] != newSymbolCellNumber)
                continue;
            wavePropagation(tmpImage, side, true, groupNumber, x, y);
            groupNumber++;
        }
    imageElements.lineGroups = groupNumber - minLineGroupNumber;
}

void ElementExtractor::findSpaces()
{
    std::copy(image, image + side * side, tmpImage);

    unsigned char groupNumber = minSpaceNumber;
    for (int y = 0; y < side; y++)
        for (int x = 0; x < side; x++) {
            if (tmpImage[side*y+x] != newSpaceCellNumber)
                continue;
            wavePropagation(tmpImage, side, false, groupNumber, x, y);
            groupNumber++;
        }
    imageElements.spaces = groupNumber - minSpaceNumber;
}
