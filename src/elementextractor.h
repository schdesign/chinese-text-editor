// elementextractor.h
// Copyright (C) 2023 Alexander Karpeko

#ifndef ELEMENT_EXTRACTOR_H
#define ELEMENT_EXTRACTOR_H

#include <cstdint>

constexpr int maxExtractorImageSide = 128;

struct ImageElements
{
    int lineGroups;
    int spaces;
};

class ElementExtractor
{
public:
    ElementExtractor(const uint64_t *image, int side,
                     ImageElements &imageElements);
    bool findElements();

private:
    void findLineGroups();
    void findSpaces();

private:
    static constexpr int maxSide = maxExtractorImageSide;
    const uint64_t *image;
    int maxWidth;
    int minLength;
    int side;
    ImageElements &imageElements;
    uint64_t tmpImage[maxSide*maxSide];
};

#endif  // ELEMENT_EXTRACTOR_H
