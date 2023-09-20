// hanzifeatures.cpp
// Copyright (C) 2023 Alexander Karpeko

#include "function.h"
#include "hanzifeatures.h"

HanziFeatures::HanziFeatures(QDialog *parent): QDialog(parent)
{
    setupUi(this);
    setGeometry(QRect(150, 100, 230, 170));

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

    int lineGroups = 0;
    int insideSpaces = 0;

    lineGroupsLineEdit->setText(QString::number(lineGroups));
    insideSpacesLineEdit->setText(QString::number(insideSpaces));
}

void HanziFeatures::accept()
{
    int lineGroups = lineGroupsLineEdit->text().toInt();
    int insideSpaces = insideSpacesLineEdit->text().toInt();
    limit(lineGroups, 0, 99);
    limit(insideSpaces, 0, 99);
    int spaces = insideSpaces + 1;
    int features = (lineGroups << 8) + spaces;

    done(features + 1);
}
