//
// Created by 13033 on 2025/6/2.
//

// You may need to build the project (run Qt uic code generator) to get "ui_mainwindow.h" resolved

#include "mainwindow.h"

// 初始化主窗口
mainwindow::mainwindow(QWidget *parent) :
    QWidget(parent), ui(new Ui::mainwindow) {
    ui->setupUi(this);    // 调用基类 QWidget 的构造函数，创建 UI 对象
}

mainwindow::~mainwindow() {
    delete ui;
}
