﻿// chinesetexteditor.cpp
// Copyright (C) 2023 Alexander Karpeko

#include "chinesetexteditor.h"
#include "elementextractor.h"
#include "hanzifeatures.h"
#include <QFileDialog>
#include <QFont>
#include <QMessageBox>
#include <QTextCharFormat>
#include <iostream>

ChineseTextEditor::ChineseTextEditor(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);

    connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(actionCloseFile, SIGNAL(triggered()), this, SLOT(closeFile()));
    connect(actionCopy, SIGNAL(triggered()), this, SLOT(copy()));
    connect(actionCut, SIGNAL(triggered()), this, SLOT(cut()));
    connect(actionFind, SIGNAL(triggered()), this, SLOT(find()));
    connect(actionHanziEvaluator, SIGNAL(triggered()), this, SLOT(evaluateHanzi()));
    connect(actionLearnHanziEvaluator, SIGNAL(triggered()), this, SLOT(learnHanziEvaluator()));
    connect(actionNewFile, SIGNAL(triggered()), this, SLOT(newFile()));
    connect(actionOpenFile, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(actionOptions, SIGNAL(triggered()), this, SLOT(options()));
    connect(actionPaste, SIGNAL(triggered()), this, SLOT(paste()));
    connect(actionQuit, SIGNAL(triggered()), this, SLOT(quit()));
    connect(actionRedo, SIGNAL(triggered()), this, SLOT(redo()));
    connect(actionSaveFile, SIGNAL(triggered()), this, SLOT(saveFile()));
    connect(actionSaveFileAs, SIGNAL(triggered()), this, SLOT(saveFileAs()));
    connect(actionUndo, SIGNAL(triggered()), this, SLOT(undo()));

    isEvaluated = false;
    learnEvaluator = false;
    learnedEvaluator = false;
    typeChineseCharacters = true;
    group = 0;
    number = 0;

    plainTextEdit->installEventFilter(this);

    QDir::setCurrent(QCoreApplication::applicationDirPath());

    try {
        translator.readFile("hanzibyfrequency.txt");
    }
    catch (const char *e) {
        QMessageBox::warning(this, tr("Error"), QString(e));
    }

    translationObjects = translator.symbolObjects();
    hanziImageNumbers.resize(translationObjects.size());
}

void ChineseTextEditor::about()
{
    QMessageBox::information(this, tr("About Chinese Text Editor"),
        tr("Chinese Text Editor\n""Copyright (C) 2023 Alexander Karpeko"));
}

void ChineseTextEditor::clearInputWord()
{
    selectorLineEdit->clear();
    monotonePinyin.clear();
    isEvaluated = false;
    group = 0;
    number = 0;
}

void ChineseTextEditor::closeFile()
{

}

void ChineseTextEditor::copy()
{

}

void ChineseTextEditor::cut()
{

}

void ChineseTextEditor::evaluateHanzi()
{
    if (!learnedEvaluator)
        return;

    HanziFeatures hanziFeatures;
    int features = hanziFeatures.exec() - 1;
    if (features < 0)
        return;

    ImageElements imageElements;
    imageElements.lineGroups = features >> 8;
    imageElements.spaces = features & 0xff;

    if (imageElements.lineGroups < 1 || imageElements.spaces < 1)
        return;

    unsigned long numbersSize = 0;
    hanziImageNumbers.resize(translationObjects.size());
    hanziEvaluator.imageNumbers(imageElements, hanziImageNumbers, numbersSize);
    hanziImageNumbers.resize(numbersSize);
    group = 0;
    selectorLineEdit->setText(translator.symbols(hanziImageNumbers, group));
    isEvaluated = true;
}

bool ChineseTextEditor::eventFilter(QObject *obj, QEvent *event)
{
    if (obj != plainTextEdit)
        return ChineseTextEditor::eventFilter(obj, event);

    if (event->type() != QEvent::KeyPress)
        return false;

    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
    int key = keyEvent->key();
    QString character = keyEvent->text().toLower();
    Qt::KeyboardModifiers modifiers = keyEvent->modifiers();

    if ((modifiers & Qt::AltModifier) && (key == Qt::Key_Q)) {
        typeChineseCharacters ^= true;
        clearInputWord();
        selectorLineEdit->setEnabled(typeChineseCharacters);
        return true;
    }

    if (!typeChineseCharacters)
        return false;

    bool isControl = key == Qt::Key_Escape || key == Qt::Key_Left || key == Qt::Key_Right;
    bool isDigit = character >= QString("1") && character <= QString("9");
    bool isLetter = character >= QString("a") && character <= QString("z");

    if (!isControl && !isDigit && !isLetter)
        return false;

    if (isLetter) {
        monotonePinyin += keyEvent->text();
        selectorLineEdit->setText(translator.symbols(monotonePinyin, group));
    }

    if (monotonePinyin.isEmpty() && !isEvaluated)
        return false;

    if (isControl) {
        switch(key) {
            case Qt::Key_Escape:
                clearInputWord();
                break;
            case Qt::Key_Left:
                if (group > 0)
                    group--;
                if (monotonePinyin.isEmpty())
                    selectorLineEdit->setText(translator.symbols(hanziImageNumbers, group));
                else
                    selectorLineEdit->setText(translator.symbols(monotonePinyin, group));
                break;
            case Qt::Key_Right:
            if (monotonePinyin.isEmpty()) {
                if ((group + 1) * translator.groupSize < hanziImageNumbers.size())
                    group++;
                selectorLineEdit->setText(translator.symbols(hanziImageNumbers, group));
            }
            else {
                if (!translator.symbols(monotonePinyin, group).isEmpty())
                    group++;
                selectorLineEdit->setText(translator.symbols(monotonePinyin, group));
            }
        }
    }

    if (isDigit) {
        number = character.toInt() - 1;
        QString hanzi;
        if (isEvaluated) {
            unsigned long n = static_cast<unsigned long>(group * translator.groupSize + number);
            unsigned long index = hanziImageNumbers[n];
            hanzi = translationObjects[index].hanzi;
        }
        else
            hanzi = translator.symbol(monotonePinyin, group, number);
        plainTextEdit->insertPlainText(hanzi);
        translationLineEdit->setText(translator.symbolData(hanzi));
        clearInputWord();
    }

    return true;
}

void ChineseTextEditor::find()
{

}

void ChineseTextEditor::learnHanziEvaluator()
{
    if (learnEvaluator)
        return;

    learnEvaluator = true;
    try {
        hanziEvaluator.init(translationObjects);
        hanziEvaluator.createVectors();
        learnedEvaluator = true;
        actionLearnHanziEvaluator->setEnabled(false);
        actionHanziEvaluator->setEnabled(true);
    }
    catch (const char *e) {
        QMessageBox::warning(this, tr("Error"), QString(e));
    }
}

void ChineseTextEditor::newFile()
{

}

void ChineseTextEditor::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open txt file"),
                       "", tr("txt files (*.txt)"));
    if (fileName.isNull()) {
        QMessageBox::warning(this, tr("Error"), tr("Filename is null"));
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return;
    QByteArray array = file.readAll();
    file.close();

    QString text(array);
    plainTextEdit->setPlainText(text);
}

void ChineseTextEditor::options()
{

}

void ChineseTextEditor::paste()
{

}

void ChineseTextEditor::quit()
{
    close();
}

void ChineseTextEditor::redo()
{

}

void ChineseTextEditor::saveFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save txt file"),
                       "", tr("txt files (*.txt)"));

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return;

    QString text = plainTextEdit->toPlainText();
    QByteArray array = text.toUtf8();

    file.write(array);
    file.close();
}

void ChineseTextEditor::saveFileAs()
{

}

void ChineseTextEditor::undo()
{

}
