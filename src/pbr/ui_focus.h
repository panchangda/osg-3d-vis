/********************************************************************************
** Form generated from reading UI file 'focus.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FOCUS_H
#define UI_FOCUS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_focus
{
public:
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QCheckBox *checkBox;

    void setupUi(QWidget *focus)
    {
        if (focus->objectName().isEmpty())
            focus->setObjectName(QString::fromUtf8("focus"));
        focus->resize(400, 300);
        horizontalLayoutWidget = new QWidget(focus);
        horizontalLayoutWidget->setObjectName(QString::fromUtf8("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(90, 110, 160, 80));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        checkBox = new QCheckBox(horizontalLayoutWidget);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));
        checkBox->setChecked(false);

        horizontalLayout->addWidget(checkBox);


        retranslateUi(focus);

        QMetaObject::connectSlotsByName(focus);
    } // setupUi

    void retranslateUi(QWidget *focus)
    {
        focus->setWindowTitle(QApplication::translate("focus", "focus", nullptr));
        checkBox->setText(QApplication::translate("focus", "focus on earth", nullptr));
    } // retranslateUi

};

namespace Ui {
    class focus: public Ui_focus {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FOCUS_H
