// chinesetexteditor.h
// Copyright (C) 2023 Alexander Karpeko

#ifndef CHINESE_TEXT_EDITOR_H
#define CHINESE_TEXT_EDITOR_H

#include "translator.h"
#include "ui_chinesetexteditor.h"
#include <QMainWindow>

class ChineseTextEditor : public QMainWindow, private Ui::ChineseTextEditor
{
    Q_OBJECT

public:
    explicit ChineseTextEditor(QWidget *parent = nullptr);

private:
    void clearInputWord();
    bool eventFilter(QObject *obj, QEvent *ev) override;

private slots:
    void about();
    void closeFile();
    void copy();
    void cut();
    void find();
    void newFile();
    void openFile();
    void options();
    void paste();
    void quit();
    void redo();
    void saveFile();
    void saveFileAs();
    void undo();

private:
    bool typeChineseCharacters;
    int group;
    int number;
    QString monotonePinyin;
    Translator translator;
    std::vector<unsigned long> paintedImageNumbers;
    std::vector<TranslationObject> translationObjects;
 };

#endif  // CHINESE_TEXT_EDITOR_H
