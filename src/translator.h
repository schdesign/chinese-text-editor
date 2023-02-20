// translator.h
// Copyright (C) 2023 Alexander Karpeko

#ifndef TRANSLATOR_H
#define TRANSLATOR_H

//#include "hanziimage.h"
#include <map>
#include <QString>
#include <vector>

struct HanziObject
{
    unsigned long frequencyRank;
    QString hanzi;
    QString monotonePinyin;
};

struct TranslationObject
{
    unsigned long frequencyRank;
    QString hanzi;
    QString pinyin;
    QString translation;
};

class Translator
{
public:
    Translator();
    void readFile(const char *filename);
    QString symbol(const QString &monotonePinyin, int group, int number);
    QString symbolData(const QString &hanzi);
    QString symbolData(const QString &monotonePinyin, int group, int number);
    const std::vector<TranslationObject>& symbolObjects();
    QString symbols(const QString &monotonePinyin, int group);
    QString symbols(std::vector<unsigned long> &numbers, int group);

public:
    static constexpr int groupSize = 9;

private:
    void init(const QString &text);
    QString monotone(const QString &pinyin);
    bool monotonePinyinToIndex(const QString &monotonePinyin, unsigned long &index);

private:
    static constexpr int characterStart = 0;
    static constexpr int letters = 27;
    static constexpr int pinyinStart = 3;
    static constexpr int translationStart = 20;
    std::map<QString, TranslationObject> hanziToObject;
    std::vector<std::vector<HanziObject>> pinyinToHanzi;
    std::vector<TranslationObject> translationObjects;
};

#endif  // TRANSLATOR_H
