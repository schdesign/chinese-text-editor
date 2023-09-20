// hanzifeatures.h
// Copyright (C) 2023 Alexander Karpeko

#ifndef HANZI_FEATURES_H
#define HANZI_FEATURES_H

#include "ui_hanzifeatures.h"
#include <QDialog>

class HanziFeatures : public QDialog, private Ui::HanziFeatures
{
    Q_OBJECT

public:
    explicit HanziFeatures(QDialog *parent = nullptr);

private slots:
    void accept();
};

#endif  // HANZI_FEATURES_H
