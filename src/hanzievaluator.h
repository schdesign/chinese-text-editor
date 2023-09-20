// hanzievaluator.h
// Copyright (C) 2023 Alexander Karpeko

#ifndef HANZI_EVALUATOR_H
#define HANZI_EVALUATOR_H

#include "elementextractor.h"
#include "hanziimage.h"
#include "translator.h"
#include <cstdint>
#include <vector>

constexpr int evaluatorImageSide = 128;
constexpr int evaluatorImageSize = evaluatorImageSide * evaluatorImageSide;

struct EvaluatorImage
{
    int lineGroups;
    int spaces;
    unsigned long frequencyRank;
    uint64_t image[evaluatorImageSize];
};

struct ImageParameters
{
    unsigned long frequencyRank;
    ImageElements elements;
    QString hanzi;
};

class HanziEvaluator
{
public:
    HanziEvaluator();
    void imageNumbers(ImageElements imageElements, std::vector<unsigned long> &numbers,
                      unsigned long &numbersSize);
    void init(const std::vector<TranslationObject> &translationObjects);
    void createVectors();

private:
    void createImages();

private:
    static constexpr int side = evaluatorImageSide;
    static constexpr int maxImagesSize = 16384;
    unsigned long imagesSize;
    std::vector<ImageParameters> imagesParameters;
    std::vector<QString> hanziSymbols;
    std::vector<EvaluatorImage> hanziImages;
};

#endif // HANZI_EVALUATOR_H
