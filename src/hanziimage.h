// hanziimage.h
// Copyright (C) 2023 Alexander Karpeko

#ifndef HANZI_IMAGE_H
#define HANZI_IMAGE_H

#include <cstdint>

constexpr int hanziImageSide = 64;
constexpr int hanziImageSize = hanziImageSide * hanziImageSide;

constexpr uint64_t minLineGroupNumber = 1;
constexpr uint64_t maxLineGroupNumber = 31;
constexpr uint64_t minSpaceNumber = 1;
constexpr uint64_t maxSpaceNumber = 63;
constexpr uint64_t newSpaceCellNumber = 0;
constexpr uint64_t newSymbolCellNumber = 1;
constexpr uint64_t lineGroupNumberOffset = 7;
constexpr uint64_t spaceNumberOffset = 1;

struct HanziImage
{
    unsigned long frequencyRank;
    uint64_t image[hanziImageSize];
};

struct ImageBorder
{
    int bottom;
    int height;
    int left;
    int right;
    int top;
    int width;
};

ImageBorder findImageBorder(uint64_t *image, int side);
void wavePropagation(uint64_t *image, int side, bool symbolCells,
                     uint64_t groupNumber, int startX, int startY);

#endif  // HANZI_IMAGE_H
