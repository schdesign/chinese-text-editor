// translator.cpp
// Copyright (C) 2023 Alexander Karpeko

#include "translator.h"
#include <QByteArray>
#include <QFile>
#include <QVector>

Translator::Translator()
{

}

void Translator::init(const QString &text)
{
    auto stringList = text.split("\n");

    TranslationObject tObj;
    unsigned long frequencyRank = 0;

    for (auto s : stringList) {
        if (s.size() <= pinyinStart)
            continue;
        tObj.frequencyRank = ++frequencyRank;
        tObj.hanzi = s.mid(characterStart, 1);
        tObj.pinyin = s.mid(pinyinStart, translationStart - pinyinStart - 1).trimmed();
        tObj.translation = s.mid(translationStart).trimmed();
        translationObjects.push_back(tObj);
    }

    HanziObject hObj;
    pinyinToHanzi.resize(letters);

    for (auto t : translationObjects) {
        if (t.pinyin.isEmpty())
            continue;

        QString monotonePinyin = monotone(t.pinyin);
        unsigned long index = letters;

        if (!monotonePinyinToIndex(monotonePinyin, index))
            continue;

        hObj.frequencyRank = t.frequencyRank;
        hObj.hanzi = t.hanzi;
        hObj.monotonePinyin = monotone(t.pinyin);
        pinyinToHanzi[index].push_back(hObj);

        hanziToObject[t.hanzi] = t;
    }
}

QString Translator::monotone(const QString &pinyin)
{
    typedef std::pair<QString, QString> p;

    std::map<QString, QString> tonemap = {
        p("ā", "a"), p("á", "a"), p("ǎ", "a"), p("à", "a"),
        p("ē", "e"), p("é", "e"), p("ě", "e"), p("è", "e"),
        p("ī", "i"), p("í", "i"), p("ǐ", "i"), p("ì", "i"),
        p("ō", "o"), p("ó", "o"), p("ǒ", "o"), p("ò", "o"),
        p("ū", "u"), p("ú", "u"), p("ǔ", "u"), p("ù", "u"),
        p("ǖ", "v"), p("ǘ", "v"), p("ǚ", "v"), p("ǜ", "v"),
        p("ü", "v")
    };

    QString monotonePinyin;

    for (auto m : pinyin) {
        QString str(m);
        if (tonemap.find(str) != tonemap.end())
            monotonePinyin += tonemap[str];
        else
            monotonePinyin += str;
    }

    return monotonePinyin;
}

bool Translator::monotonePinyinToIndex(const QString &monotonePinyin, unsigned long &index)
{
    if (monotonePinyin[0] == QString("ü"))
        index = letters - 1;
    else
        index = monotonePinyin[0].unicode() - 'a';

    if (index > letters - 1)
        return false;

    return true;
}

void Translator::readFile(const char *filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        throw "Translator readFile() error";
    QByteArray array = file.readAll();
    file.close();

    QString text(array);
    init(text);
}

QString Translator::symbol(const QString &monotonePinyin, int group, int number)
{
    QString hanzi;
    unsigned long index = letters;
    if (!monotonePinyinToIndex(monotonePinyin, index))
        return hanzi;

    int n = 0;
    int n2 = groupSize * group + number;
    for (auto p : pinyinToHanzi[index]) {
        if (p.monotonePinyin.startsWith(monotonePinyin)) {
            if (n == n2) {
                hanzi = p.hanzi;
                break;
            }
            n++;
        }
    }

    return hanzi;
}

QString Translator::symbolData(const QString &hanzi)
{
    QString data;

    if (hanziToObject.find(hanzi) == hanziToObject.end())
        return data;

    TranslationObject tObj = hanziToObject[hanzi];

    data = QString("%1  %2  %3").arg(tObj.hanzi).arg(tObj.pinyin).arg(tObj.translation);

    return data;
}

QString Translator::symbolData(const QString &monotonePinyin, int group, int number)
{
    QString hanzi = symbol(monotonePinyin, group, number);
    QString data = symbolData(hanzi);

    return data;
}

const std::vector<TranslationObject>& Translator::symbolObjects()
{
    const auto &ref = translationObjects;
    return ref;
}

QString Translator::symbols(const QString &monotonePinyin, int group)
{
    QString hanziGroup;
    QString str;

    unsigned long index = letters;
    if (!monotonePinyinToIndex(monotonePinyin, index))
        return hanziGroup;

    int n = 0;
    int n2 = groupSize * group;
    for (auto p : pinyinToHanzi[index]) {
        if (p.monotonePinyin.startsWith(monotonePinyin)) {
            if (n >= n2) {
                hanziGroup += QString("%1. %2   ").arg(n - n2 + 1).arg(p.hanzi);
                if (n == n2 + groupSize - 1)
                    break;
            }
            n++;
        }
    }

    return hanziGroup;
}

QString Translator::symbols(std::vector<unsigned long> &numbers, int group)
{
    QString hanziGroup;
    unsigned long start = static_cast<unsigned long>(groupSize * group);

    for (unsigned long i = start; i < start + groupSize && i < numbers.size(); i++) {
        unsigned long j = numbers[i];
        hanziGroup += QString("%1. %2   ").arg(i - start + 1).arg(translationObjects[j].hanzi);
    }

    return hanziGroup;
}
