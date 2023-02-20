// main.cpp
// Copyright (C) 2023 Alexander Karpeko

#include "chinesetexteditor.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ChineseTextEditor w;
    w.show();
    return a.exec();
}
