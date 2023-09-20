// hanzievaluator.cpp
// Copyright (C) 2023 Alexander Karpeko

#include "hanzievaluator.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <QFont>
#include <QImage>
#include <QString>
#include <QPainter>
#include <iostream>

HanziEvaluator::HanziEvaluator()
{

}

void HanziEvaluator::createImages()
{
    QImage image(side, side, QImage::Format_Mono);
    QPainter painter(&image);
    QFont font("Sans", 85, QFont::Normal);
    painter.setFont(font);
    painter.setPen(Qt::black);
    const QRect paintRectangle = QRect(0, 0, side, side);
    uint64_t symbolImage[side*side];

    hanziImages.resize(imagesSize);

    for (unsigned long i = 0; i < hanziImages.size(); i++) {
        image.fill(1);
        painter.drawText(paintRectangle, Qt::AlignCenter, hanziSymbols[i]);

        for (int y = 0; y < side; y++)
            for (int x = 0; x < side; x++) {
                symbolImage[side*y+x] = 0;
                if (image.pixelColor(x, y) == Qt::black)
                    symbolImage[side*y+x] = 1;
            }

        ImageBorder imageBorder = findImageBorder(symbolImage, side);
        int startX = (side - imageBorder.width) / 2;
        int startY = (side - imageBorder.height) / 2;

        if (startX < 0) {
            std::cerr << "Error: startX = " << startX << std::endl;
            startX = 0;
        }

        if (startY < 0) {
            std::cerr << "Error: startY = " << startY << std::endl;
            startY = 0;
        }

        int dx = startX - imageBorder.left;
        int dy = startY - imageBorder.top;

        hanziImages[i].frequencyRank = i;

        std::fill(hanziImages[i].image, hanziImages[i].image + side * side, 0);

        for (int y = imageBorder.top; y <= imageBorder.bottom; y++)
            for (int x = imageBorder.left; x <= imageBorder.right; x++)
                if (symbolImage[side*y+x] != 0)
                    hanziImages[i].image[side*(y+dy)+x+dx] = 1;
    }
}

void HanziEvaluator::createVectors()
{
    imagesParameters.resize(imagesSize);
    unsigned long i;

    try {
        for (i = 0; i < imagesParameters.size(); i++) {
            imagesParameters[i].frequencyRank = i;
            imagesParameters[i].hanzi = hanziSymbols[i];
            ImageElements imageElements;
            ElementExtractor elementExtractor(hanziImages[i].image, side, imageElements);
            elementExtractor.findElements();
            imagesParameters[i].elements = imageElements;
        }
    }
    catch (const char *e) {
        throw QString(QString(e) + QString(" frequencyRank = %1").arg(i)).toStdString().c_str();
    }
}

void HanziEvaluator::imageNumbers(ImageElements imageElements, std::vector<unsigned long> &numbers,
                                  unsigned long &numbersSize)
{
    std::fill(numbers.begin(), numbers.end(), 0);
    numbersSize = 0;

    for (unsigned long i = 0; i < imagesParameters.size(); i++) {
        if (numbersSize >= numbers.size())
            break;
        if (imagesParameters[i].elements.lineGroups == imageElements.lineGroups &&
            imagesParameters[i].elements.spaces == imageElements.spaces)
            numbers[numbersSize++] = i;
    }
}

void HanziEvaluator::init(const std::vector<TranslationObject> &translationObjects)
{
    imagesSize = translationObjects.size();

    if (imagesSize == 0)
        throw "translationObjects is empty.";

    if (imagesSize > maxImagesSize)
        throw "translationObjects size is more than maxOutputSize.";

    for (unsigned long i = 0; i < imagesSize; i++)
        hanziSymbols.push_back(translationObjects[i].hanzi);

    createImages();
}
