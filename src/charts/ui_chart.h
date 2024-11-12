/********************************************************************************
** Form generated from reading UI file 'chart.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHART_H
#define UI_CHART_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Chart
{
public:
    QPushButton *ButtonAxis;
    QPushButton *ButtonColumn;
    QPushButton *ButtonDot;
    QPushButton *ButtonPolyline;
    QPushButton *ButtonPie;
    QPushButton *ButtonCurve;
    QPushButton *ButtonRadar;
    QPushButton *ButtonParallel;
    QPushButton *ButtonExit;

    void setupUi(QWidget *Chart)
    {
        if (Chart->objectName().isEmpty())
            Chart->setObjectName(QString::fromUtf8("Chart"));
        Chart->resize(400, 300);
        ButtonAxis = new QPushButton(Chart);
        ButtonAxis->setObjectName(QString::fromUtf8("ButtonAxis"));
        ButtonAxis->setGeometry(QRect(30, 10, 81, 21));
        ButtonColumn = new QPushButton(Chart);
        ButtonColumn->setObjectName(QString::fromUtf8("ButtonColumn"));
        ButtonColumn->setGeometry(QRect(40, 160, 80, 20));
        ButtonDot = new QPushButton(Chart);
        ButtonDot->setObjectName(QString::fromUtf8("ButtonDot"));
        ButtonDot->setGeometry(QRect(160, 160, 80, 20));
        ButtonPolyline = new QPushButton(Chart);
        ButtonPolyline->setObjectName(QString::fromUtf8("ButtonPolyline"));
        ButtonPolyline->setGeometry(QRect(270, 160, 80, 20));
        ButtonPie = new QPushButton(Chart);
        ButtonPie->setObjectName(QString::fromUtf8("ButtonPie"));
        ButtonPie->setGeometry(QRect(40, 200, 80, 20));
        ButtonCurve = new QPushButton(Chart);
        ButtonCurve->setObjectName(QString::fromUtf8("ButtonCurve"));
        ButtonCurve->setGeometry(QRect(160, 200, 80, 20));
        ButtonRadar = new QPushButton(Chart);
        ButtonRadar->setObjectName(QString::fromUtf8("ButtonRadar"));
        ButtonRadar->setGeometry(QRect(270, 200, 80, 20));
        ButtonParallel = new QPushButton(Chart);
        ButtonParallel->setObjectName(QString::fromUtf8("ButtonParallel"));
        ButtonParallel->setGeometry(QRect(40, 240, 311, 20));
        ButtonExit = new QPushButton(Chart);
        ButtonExit->setObjectName(QString::fromUtf8("ButtonExit"));
        ButtonExit->setGeometry(QRect(170, 270, 61, 20));

        retranslateUi(Chart);

        QMetaObject::connectSlotsByName(Chart);
    } // setupUi

    void retranslateUi(QWidget *Chart)
    {
        Chart->setWindowTitle(QApplication::translate("Chart", "Form", nullptr));
        ButtonAxis->setText(QApplication::translate("Chart", "\346\230\276\347\244\272\345\235\220\346\240\207\350\275\264", nullptr));
        ButtonColumn->setText(QApplication::translate("Chart", "\344\270\211\347\273\264\346\237\261\347\212\266\345\233\276", nullptr));
        ButtonDot->setText(QApplication::translate("Chart", "\344\270\211\347\273\264\346\225\243\347\202\271\345\233\276", nullptr));
        ButtonPolyline->setText(QApplication::translate("Chart", "\344\270\211\347\273\264\346\212\230\347\272\277\345\233\276", nullptr));
        ButtonPie->setText(QApplication::translate("Chart", "\344\270\211\347\273\264\351\245\274\347\212\266\345\233\276", nullptr));
        ButtonCurve->setText(QApplication::translate("Chart", "\344\270\211\347\273\264\346\233\262\347\272\277\345\233\276", nullptr));
        ButtonRadar->setText(QApplication::translate("Chart", "\344\270\211\347\273\264\351\233\267\350\276\276\345\233\276", nullptr));
        ButtonParallel->setText(QApplication::translate("Chart", "\344\270\211\347\273\264\345\271\263\350\241\214\345\235\220\346\240\207\345\233\276", nullptr));
        ButtonExit->setText(QApplication::translate("Chart", "\351\200\200\345\207\272", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Chart: public Ui_Chart {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHART_H
