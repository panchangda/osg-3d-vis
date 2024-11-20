/********************************************************************************
** Form generated from reading UI file 'main_window.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAIN_WINDOW_H
#define UI_MAIN_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_main_window
{
public:
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *StreamlineButton;
    QPushButton *LICButton;
    QPushButton *pushButton_5;
    QPushButton *pushButton_7;
    QPushButton *pushButton_6;
    QPushButton *pushButton_4;
    QPushButton *pushButton_3;

    void setupUi(QWidget *main_window)
    {
        if (main_window->objectName().isEmpty())
            main_window->setObjectName(QString::fromUtf8("main_window"));
        main_window->resize(705, 552);
        horizontalLayoutWidget = new QWidget(main_window);
        horizontalLayoutWidget->setObjectName(QString::fromUtf8("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(10, 430, 681, 101));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        StreamlineButton = new QPushButton(horizontalLayoutWidget);
        StreamlineButton->setObjectName(QString::fromUtf8("StreamlineButton"));

        horizontalLayout->addWidget(StreamlineButton);

        LICButton = new QPushButton(horizontalLayoutWidget);
        LICButton->setObjectName(QString::fromUtf8("LICButton"));

        horizontalLayout->addWidget(LICButton);

        pushButton_5 = new QPushButton(horizontalLayoutWidget);
        pushButton_5->setObjectName(QString::fromUtf8("pushButton_5"));

        horizontalLayout->addWidget(pushButton_5);

        pushButton_7 = new QPushButton(horizontalLayoutWidget);
        pushButton_7->setObjectName(QString::fromUtf8("pushButton_7"));

        horizontalLayout->addWidget(pushButton_7);

        pushButton_6 = new QPushButton(horizontalLayoutWidget);
        pushButton_6->setObjectName(QString::fromUtf8("pushButton_6"));

        horizontalLayout->addWidget(pushButton_6);

        pushButton_4 = new QPushButton(horizontalLayoutWidget);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));

        horizontalLayout->addWidget(pushButton_4);

        pushButton_3 = new QPushButton(horizontalLayoutWidget);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));

        horizontalLayout->addWidget(pushButton_3);


        retranslateUi(main_window);

        QMetaObject::connectSlotsByName(main_window);
    } // setupUi

    void retranslateUi(QWidget *main_window)
    {
        main_window->setWindowTitle(QApplication::translate("main_window", "Form", nullptr));
        StreamlineButton->setText(QApplication::translate("main_window", "Streamline", nullptr));
        LICButton->setText(QApplication::translate("main_window", "LIC", nullptr));
        pushButton_5->setText(QApplication::translate("main_window", "PushButton", nullptr));
        pushButton_7->setText(QApplication::translate("main_window", "PushButton", nullptr));
        pushButton_6->setText(QApplication::translate("main_window", "PushButton", nullptr));
        pushButton_4->setText(QApplication::translate("main_window", "PushButton", nullptr));
        pushButton_3->setText(QApplication::translate("main_window", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class main_window: public Ui_main_window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAIN_WINDOW_H
