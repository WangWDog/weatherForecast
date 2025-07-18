//
// Created by 13033 on 2025/6/2.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class mainwindow; }
QT_END_NAMESPACE

class mainwindow : public QWidget {
Q_OBJECT

public:
    // explicit关键字防止隐式类型转换
    // parent参数指定父窗口部件
    explicit mainwindow(QWidget *parent = nullptr);
    // override关键字确保正确覆盖基类虚函数
    ~mainwindow() override;

private:
    Ui::mainwindow *ui;
};


#endif //MAINWINDOW_H
