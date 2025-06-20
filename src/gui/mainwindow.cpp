//
// Created by 13033 on 2025/6/2.
//

// You may need to build the project (run Qt uic code generator) to get "ui_mainwindow.h" resolved

#include "mainwindow.h"
#include "ui_mainwindow.h"

// 初始化主窗口
mainwindow::mainwindow(QWidget *parent) :
    QWidget(parent), ui(new Ui::mainwindow) {
    ui->setupUi(this);
    connect(&process, &QProcess::readyReadStandardOutput, this, &mainwindow::readOutput);
    connect(ui->runButton, &QPushButton::clicked, this, &mainwindow::runCommand);
}

mainwindow::~mainwindow() {
    process.kill();
    process.waitForFinished();
    delete ui;
}

void mainwindow::runCommand() {
    QString cmd = ui->commandEdit->text();
    ui->outputEdit->clear();
    QStringList args = cmd.split(' ', Qt::SkipEmptyParts);
    process.start("./weather_cli", args);
}

void mainwindow::readOutput() {
    ui->outputEdit->append(QString::fromUtf8(process.readAllStandardOutput()));
}
