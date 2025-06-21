//
// Created by 13033 on 2025/6/2.
//

// You may need to build the project (run Qt uic code generator) to get "ui_mainwindow.h" resolved

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cli_interface.h"
#include <iostream>
#include <sstream>
// 初始化主窗口
mainwindow::mainwindow(QWidget *parent) :
    QWidget(parent), ui(new Ui::mainwindow) {
    ui->setupUi(this);
    connect(ui->runButton, &QPushButton::clicked, this, &mainwindow::runCommand);
}

mainwindow::~mainwindow() {
    delete ui;
}

void mainwindow::runCommand() {
    QString cmd = ui->commandEdit->text();
    ui->outputEdit->clear();
    QStringList argList = cmd.split(' ', Qt::SkipEmptyParts);

    std::vector<std::string> args;
    args.push_back("weather_gui");
    for (const auto& a : argList) {
        args.push_back(a.toStdString());
    }

    std::vector<char*> argv;
    for (auto& a : args) {
        argv.push_back(a.data());
    }

    std::ostringstream oss;
    auto* oldBuf = std::cout.rdbuf(oss.rdbuf());
    run_cli(static_cast<int>(argv.size()), argv.data());
    std::cout.rdbuf(oldBuf);
    ui->outputEdit->append(QString::fromStdString(oss.str()));
}
