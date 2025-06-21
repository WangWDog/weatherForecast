#include "mainwindow.h"
#include "cli_interface.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QStatusBar>
#include <sstream>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowTitle("Weather Forecast");
    this->setMinimumSize(800, 600);

    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    // Top layout
    QHBoxLayout *topLayout = new QHBoxLayout();
    cityInput = new QLineEdit();
    cityInput->setPlaceholderText("Enter city name...");
    languageBox = new QComboBox();
    languageBox->addItems({"English", "中文"});
    unitBox = new QComboBox();
    unitBox->addItems({"Celsius / km/h", "Fahrenheit / mph"});
    dateFormatBox = new QComboBox();
    dateFormatBox->addItems({"YYYY-MM-DD", "DD/MM/YYYY"});
    settingsButton = new QPushButton("\xE2\x9A\x99 Settings");

    topLayout->addWidget(new QLabel("City:"));
    topLayout->addWidget(cityInput);
    topLayout->addWidget(new QLabel("Language:"));
    topLayout->addWidget(languageBox);
    topLayout->addWidget(new QLabel("Units:"));
    topLayout->addWidget(unitBox);
    topLayout->addWidget(new QLabel("Date Format:"));
    topLayout->addWidget(dateFormatBox);
    topLayout->addWidget(settingsButton);

    // Middle layout
    QHBoxLayout *midLayout = new QHBoxLayout();

    // Weather Info Box
    QGroupBox *weatherBox = new QGroupBox("Weather Info");
    QVBoxLayout *weatherLayout = new QVBoxLayout();
    weatherLabel = new QLabel("\xF0\x9F\x8C\xA4 25\xC2\xB0C, Clear Skies");
    weatherLabel->setAlignment(Qt::AlignCenter);
    weatherLayout->addWidget(weatherLabel);
    weatherBox->setLayout(weatherLayout);

    // Right Panel
    QVBoxLayout *rightPanel = new QVBoxLayout();
    aiBox = new QTextBrowser();
    aiBox->setPlaceholderText("AI Suggestion: Bring an umbrella if you're heading out...");
    indexList = new QListWidget();
    indexList->addItems({"\xE2\x98\x80 UV Index: Moderate", "\xF0\x9F\x91\x95 Clothing: Light Jacket", "\xF0\x9F\x8C\xAC Wind: Mild Breeze"});

    rightPanel->addWidget(new QLabel("AI Suggestions"));
    rightPanel->addWidget(aiBox);
    rightPanel->addWidget(new QLabel("Weather Index"));
    rightPanel->addWidget(indexList);

    midLayout->addWidget(weatherBox, 2);
    midLayout->addLayout(rightPanel, 3);

    // Status bar
    QStatusBar *status = new QStatusBar();
    status->showMessage("Last updated: --:--");
    this->setStatusBar(status);

    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(midLayout);
    this->setCentralWidget(central);

    connect(settingsButton, &QPushButton::clicked, this, &MainWindow::refreshWeather);
}

MainWindow::~MainWindow() {}

void MainWindow::refreshWeather()
{
    // Build CLI arguments to reuse existing logic
    std::vector<std::string> args;
    args.push_back("weather_gui");
    args.push_back("show_forecast");

    if (unitBox->currentIndex() == 1)
        args.emplace_back("--unit=imperial");
    else
        args.emplace_back("--unit=metric");

    std::vector<char*> argv;
    for (auto& a : args) {
        argv.push_back(a.data());
    }

    std::ostringstream oss;
    auto* old = std::cout.rdbuf(oss.rdbuf());
    run_cli(static_cast<int>(argv.size()), argv.data());
    std::cout.rdbuf(old);

    aiBox->setPlainText(QString::fromStdString(oss.str()));
}
