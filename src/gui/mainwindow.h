#pragma once
#include <QMainWindow>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QTextBrowser>
#include <QListWidget>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
private slots:
    void refreshWeather();
private:
    QLineEdit* cityInput;
    QComboBox* languageBox;
    QComboBox* unitBox;
    QComboBox* dateFormatBox;
    QPushButton* settingsButton;
    QLabel* weatherLabel;
    QTextBrowser* aiBox;
    QListWidget* indexList;
};
