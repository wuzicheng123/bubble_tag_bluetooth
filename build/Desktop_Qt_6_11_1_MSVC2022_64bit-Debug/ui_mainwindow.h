/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>
#include "mycustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout_4;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_3;
    QLabel *label_3;
    QLabel *missionIDLabel;
    QLabel *label_4;
    QLabel *detectResultLabel;
    QLabel *label_5;
    QLabel *pressureLabel;
    QLabel *label_8;
    QLabel *diameterLabel;
    QLabel *label_12;
    QLabel *resultLabel;
    QGridLayout *gridLayout;
    QPushButton *scanButton;
    QPushButton *connectButton;
    QPushButton *clearButton;
    QListWidget *deviceList;
    QTreeWidget *serviceTree;
    MyCustomPlot *plotWidget;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QPushButton *writeButton;
    QPushButton *subscribeButton;
    QPushButton *saveButton;
    QPushButton *setPathButton;
    QMenuBar *menuBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1000, 700);
        MainWindow->setStyleSheet(QString::fromUtf8("\n"
"/* ========== \345\205\250\345\261\200\346\240\267\345\274\217 ========== */\n"
"* {\n"
"    font-family: \"Microsoft YaHei\", \"Segoe UI\", \"Roboto\";\n"
"    font-size: 11pt;\n"
"    outline: none; /* \347\247\273\351\231\244\347\204\246\347\202\271\350\231\232\347\272\277\346\241\206 */\n"
"}\n"
"\n"
"QMainWindow {\n"
"    background-color: #f5f7fa;\n"
"}\n"
"\n"
"QWidget#centralWidget {\n"
"    background-color: transparent;\n"
"}\n"
"\n"
"/* ========== \346\214\211\351\222\256\346\240\267\345\274\217 ========== */\n"
"QPushButton {\n"
"    background-color: #0078d4;\n"
"    color: white;\n"
"    border: none;\n"
"    border-radius: 6px;\n"
"    padding: 8px 16px;\n"
"    font-weight: bold;\n"
"    transition: background 0.2s;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: #106ebe;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: #005a9e;\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"    background-color: #c7c7c7;\n"
"    color: #6e6e6e;\n"
"}\n"
"\n"
"/* ========== \345\210\206"
                        "\347\273\204\346\241\206\346\240\267\345\274\217 ========== */\n"
"QGroupBox {\n"
"    background-color: #ffffff;\n"
"    border: 1px solid #e1e5eb;\n"
"    border-radius: 10px;\n"
"    margin-top: 12px;\n"
"    padding-top: 12px;\n"
"    font-weight: bold;\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 16px;\n"
"    top: 0px;\n"
"    padding: 0 12px;\n"
"    background-color: #ffffff;\n"
"    color: #0078d4;\n"
"}\n"
"\n"
"/* ========== \345\210\227\350\241\250\346\216\247\344\273\266 ========== */\n"
"QListWidget, QTreeWidget {\n"
"    background-color: #ffffff;\n"
"    border: 1px solid #d4d8dd;\n"
"    border-radius: 8px;\n"
"    padding: 4px;\n"
"    outline: none;\n"
"}\n"
"\n"
"QListWidget::item, QTreeWidget::item {\n"
"    padding: 6px 8px;\n"
"    border-bottom: 1px solid #f0f0f0;\n"
"    border-radius: 4px;\n"
"}\n"
"\n"
"QListWidget::item:selected, QTreeWidget::item:selected {\n"
"    background-color: #e1f0fa;\n"
"    color: #0078d4;\n"
"}\n"
"\n"
"QListWidget::i"
                        "tem:hover, QTreeWidget::item:hover {\n"
"    background-color: #f5f5f5;\n"
"}\n"
"\n"
"/* ========== \350\241\250\346\240\274\350\247\206\345\233\276 ========== */\n"
"QTableView {\n"
"    border: 1px solid #d4d8dd;\n"
"    border-radius: 8px;\n"
"    background-color: white;\n"
"    alternate-background-color: #fafbfc;\n"
"    selection-background-color: #0078d4;\n"
"}\n"
"\n"
"QTableView::item {\n"
"    padding: 6px;\n"
"    border-bottom: 1px solid #eef2f5;\n"
"}\n"
"\n"
"QTableView::item:selected {\n"
"    background-color: #0078d4;\n"
"    color: white;\n"
"}\n"
"\n"
"QHeaderView::section {\n"
"    background-color: #f2f4f8;\n"
"    padding: 8px;\n"
"    border: none;\n"
"    border-right: 1px solid #e1e5eb;\n"
"    border-bottom: 1px solid #e1e5eb;\n"
"    font-weight: bold;\n"
"    color: #2c3e50;\n"
"}\n"
"\n"
"/* ========== \350\217\234\345\215\225\346\240\267\345\274\217 ========== */\n"
"QMenuBar {\n"
"    background-color: #ffffff;\n"
"    border-bottom: 1px solid #e1e5eb;\n"
"}\n"
"\n"
"QMenuBar::"
                        "item {\n"
"    padding: 6px 12px;\n"
"    background-color: transparent;\n"
"    border-radius: 4px;\n"
"}\n"
"\n"
"QMenuBar::item:selected {\n"
"    background-color: #e1f0fa;\n"
"    color: #0078d4;\n"
"}\n"
"\n"
"QMenu {\n"
"    background-color: white;\n"
"    border: 1px solid #d4d8dd;\n"
"    border-radius: 6px;\n"
"    padding: 4px;\n"
"}\n"
"\n"
"QMenu::item {\n"
"    padding: 6px 24px;\n"
"    border-radius: 4px;\n"
"}\n"
"\n"
"QMenu::item:selected {\n"
"    background-color: #0078d4;\n"
"    color: white;\n"
"}\n"
"\n"
"QMenu::separator {\n"
"    height: 1px;\n"
"    background-color: #e1e5eb;\n"
"    margin: 4px 8px;\n"
"}\n"
"\n"
"/* ========== \346\240\207\347\255\276\346\240\267\345\274\217 ========== */\n"
"QLabel {\n"
"    color: #2c3e50;\n"
"}\n"
"\n"
"QLabel[class=\"titleLabel\"] {\n"
"    font-size: 14pt;\n"
"    font-weight: bold;\n"
"    color: #0078d4;\n"
"}\n"
"\n"
"/* ========== \350\207\252\345\256\232\344\271\211\347\273\230\345\233\276\346\216\247\344\273\266\350\203\214\346\231\257 "
                        "========== */\n"
"QWidget#plotWidget {\n"
"    background-color: #ffffff;\n"
"    border: 1px solid #e1e5eb;\n"
"    border-radius: 10px;\n"
"}\n"
"   "));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName("centralWidget");
        gridLayout_4 = new QGridLayout(centralWidget);
        gridLayout_4->setSpacing(12);
        gridLayout_4->setObjectName("gridLayout_4");
        gridLayout_4->setContentsMargins(12, 12, 12, 12);
        groupBox_2 = new QGroupBox(centralWidget);
        groupBox_2->setObjectName("groupBox_2");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy);
        groupBox_2->setMinimumSize(QSize(320, 0));
        groupBox_2->setFlat(false);
        groupBox_2->setCheckable(false);
        gridLayout_3 = new QGridLayout(groupBox_2);
        gridLayout_3->setSpacing(8);
        gridLayout_3->setObjectName("gridLayout_3");
        gridLayout_3->setContentsMargins(12, 16, 12, 12);
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName("label_3");

        gridLayout_3->addWidget(label_3, 0, 0, 1, 1);

        missionIDLabel = new QLabel(groupBox_2);
        missionIDLabel->setObjectName("missionIDLabel");
        missionIDLabel->setStyleSheet(QString::fromUtf8("font-weight: bold; color: #0078d4;"));

        gridLayout_3->addWidget(missionIDLabel, 0, 1, 1, 4);

        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName("label_4");

        gridLayout_3->addWidget(label_4, 1, 0, 1, 1);

        detectResultLabel = new QLabel(groupBox_2);
        detectResultLabel->setObjectName("detectResultLabel");
        detectResultLabel->setStyleSheet(QString::fromUtf8("font-weight: bold;"));

        gridLayout_3->addWidget(detectResultLabel, 1, 1, 1, 4);

        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName("label_5");

        gridLayout_3->addWidget(label_5, 2, 0, 1, 1);

        pressureLabel = new QLabel(groupBox_2);
        pressureLabel->setObjectName("pressureLabel");

        gridLayout_3->addWidget(pressureLabel, 2, 1, 1, 1);

        label_8 = new QLabel(groupBox_2);
        label_8->setObjectName("label_8");

        gridLayout_3->addWidget(label_8, 2, 2, 1, 1);

        diameterLabel = new QLabel(groupBox_2);
        diameterLabel->setObjectName("diameterLabel");

        gridLayout_3->addWidget(diameterLabel, 2, 3, 1, 1);

        label_12 = new QLabel(groupBox_2);
        label_12->setObjectName("label_12");

        gridLayout_3->addWidget(label_12, 3, 0, 1, 1);

        resultLabel = new QLabel(groupBox_2);
        resultLabel->setObjectName("resultLabel");
        resultLabel->setStyleSheet(QString::fromUtf8("font-size: 12pt; font-weight: bold; color: #d83b01;"));

        gridLayout_3->addWidget(resultLabel, 3, 1, 1, 4);


        gridLayout_4->addWidget(groupBox_2, 1, 1, 1, 1);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(8);
        gridLayout->setObjectName("gridLayout");
        scanButton = new QPushButton(centralWidget);
        scanButton->setObjectName("scanButton");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(scanButton->sizePolicy().hasHeightForWidth());
        scanButton->setSizePolicy(sizePolicy1);
        scanButton->setMinimumSize(QSize(100, 0));

        gridLayout->addWidget(scanButton, 0, 0, 1, 1);

        connectButton = new QPushButton(centralWidget);
        connectButton->setObjectName("connectButton");
        sizePolicy1.setHeightForWidth(connectButton->sizePolicy().hasHeightForWidth());
        connectButton->setSizePolicy(sizePolicy1);
        connectButton->setMinimumSize(QSize(100, 0));

        gridLayout->addWidget(connectButton, 0, 1, 1, 1);

        clearButton = new QPushButton(centralWidget);
        clearButton->setObjectName("clearButton");
        sizePolicy1.setHeightForWidth(clearButton->sizePolicy().hasHeightForWidth());
        clearButton->setSizePolicy(sizePolicy1);
        clearButton->setMinimumSize(QSize(80, 0));

        gridLayout->addWidget(clearButton, 0, 2, 1, 1);

        deviceList = new QListWidget(centralWidget);
        deviceList->setObjectName("deviceList");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(deviceList->sizePolicy().hasHeightForWidth());
        deviceList->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(deviceList, 1, 0, 1, 3);

        serviceTree = new QTreeWidget(centralWidget);
        serviceTree->setObjectName("serviceTree");
        sizePolicy2.setHeightForWidth(serviceTree->sizePolicy().hasHeightForWidth());
        serviceTree->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(serviceTree, 2, 0, 1, 3);


        gridLayout_4->addLayout(gridLayout, 0, 0, 1, 1);

        plotWidget = new MyCustomPlot(centralWidget);
        plotWidget->setObjectName("plotWidget");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy3.setHorizontalStretch(1);
        sizePolicy3.setVerticalStretch(1);
        sizePolicy3.setHeightForWidth(plotWidget->sizePolicy().hasHeightForWidth());
        plotWidget->setSizePolicy(sizePolicy3);

        gridLayout_4->addWidget(plotWidget, 0, 1, 1, 1);

        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName("groupBox");
        sizePolicy.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy);
        groupBox->setMinimumSize(QSize(320, 0));
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setSpacing(10);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setContentsMargins(12, 16, 12, 12);
        writeButton = new QPushButton(groupBox);
        writeButton->setObjectName("writeButton");
        writeButton->setMinimumSize(QSize(90, 32));

        gridLayout_2->addWidget(writeButton, 0, 0, 1, 1);

        subscribeButton = new QPushButton(groupBox);
        subscribeButton->setObjectName("subscribeButton");
        subscribeButton->setMinimumSize(QSize(90, 32));

        gridLayout_2->addWidget(subscribeButton, 0, 1, 1, 1);

        saveButton = new QPushButton(groupBox);
        saveButton->setObjectName("saveButton");
        saveButton->setMinimumSize(QSize(90, 32));

        gridLayout_2->addWidget(saveButton, 1, 0, 1, 1);

        setPathButton = new QPushButton(groupBox);
        setPathButton->setObjectName("setPathButton");
        setPathButton->setMinimumSize(QSize(90, 32));

        gridLayout_2->addWidget(setPathButton, 1, 1, 1, 1);


        gridLayout_4->addWidget(groupBox, 1, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName("menuBar");
        menuBar->setGeometry(QRect(0, 0, 1000, 33));
        MainWindow->setMenuBar(menuBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "bubble_Tag - \345\256\211\345\205\250\351\230\200\345\257\206\345\260\201\346\243\200\346\265\213\347\263\273\347\273\237", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "\346\243\200\346\265\213\347\273\223\346\236\234", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\344\270\232\345\212\241\346\240\207\347\255\276\357\274\232", nullptr));
        missionIDLabel->setText(QString());
        label_4->setText(QCoreApplication::translate("MainWindow", "\346\243\200\346\265\213\347\273\223\346\236\234\357\274\232", nullptr));
        detectResultLabel->setText(QString());
        label_5->setText(QCoreApplication::translate("MainWindow", "\345\216\213\345\212\233\357\274\232", nullptr));
        pressureLabel->setText(QString());
        label_8->setText(QCoreApplication::translate("MainWindow", "\347\233\264\345\276\204\357\274\232", nullptr));
        diameterLabel->setText(QString());
        label_12->setText(QCoreApplication::translate("MainWindow", "\346\234\200\347\273\210\347\273\223\346\236\234\357\274\232", nullptr));
        resultLabel->setText(QString());
        scanButton->setText(QCoreApplication::translate("MainWindow", "\346\211\253\346\217\217\350\256\276\345\244\207", nullptr));
        connectButton->setText(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245\350\256\276\345\244\207", nullptr));
        clearButton->setText(QCoreApplication::translate("MainWindow", "\346\270\205\345\233\276", nullptr));
        QTreeWidgetItem *___qtreewidgetitem = serviceTree->headerItem();
        ___qtreewidgetitem->setText(0, QCoreApplication::translate("MainWindow", "\346\234\215\345\212\241 / \347\211\271\345\276\201\345\200\274", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "\347\211\271\345\276\201\345\200\274\346\223\215\344\275\234", nullptr));
        writeButton->setText(QCoreApplication::translate("MainWindow", "\350\256\276\347\275\256\345\275\223\345\211\215\346\227\266\351\227\264", nullptr));
        subscribeButton->setText(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245\346\234\215\345\212\241", nullptr));
        saveButton->setText(QCoreApplication::translate("MainWindow", "\345\257\274\345\207\272\346\212\245\345\221\212", nullptr));
        setPathButton->setText(QCoreApplication::translate("MainWindow", "\350\256\276\347\275\256\350\267\257\345\276\204", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
