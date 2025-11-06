#include "mainwindow.h"
#include <QVBoxLayout>
#include <QDateTime>
#include <QFont>
#include <QPalette>
#include <QTime>
#include <QPainter>
#include <QtMath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      timeZoneComboBox(new QComboBox(this)),
      filterEdit(new QLineEdit(this)),
      timeLabel(new QLabel(this)),
      dateLabel(new QLabel(this)),
      offsetLabel(new QLabel(this)),
      themeButton(new QPushButton(this)),
      systemTzButton(new QPushButton(this)),
      autoThemeCheckBox(new QCheckBox("自动昼夜模式", this)),
      timer(new QTimer(this)),
      settings(new QSettings("MyCompany", "WorldClockApp", this))
{
    // --- 加载地图 ---
    //worldMap.load(":/images/worldmap.png");

    // --- 主界面 ---
    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);

    // --- 搜索栏 ---
    filterEdit->setPlaceholderText("🔍 搜索时区（如 Europe、Shanghai、UTC...）");
    filterEdit->setClearButtonEnabled(true);
    filterEdit->setFixedHeight(28);

    // --- 加载时区列表 ---
    loadTimeZones();
    timeZoneComboBox->setMinimumWidth(250);

    // --- 控件布局 ---
    themeButton->setText("🌙 夜间模式");
    themeButton->setFixedWidth(120);
    systemTzButton->setText("🧭 使用系统时区");
    systemTzButton->setFixedWidth(150);
    autoThemeCheckBox->setChecked(true);

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(filterEdit);
    topLayout->addWidget(timeZoneComboBox);
    topLayout->addWidget(themeButton);
    topLayout->addWidget(systemTzButton);
    topLayout->addWidget(autoThemeCheckBox);

    // --- 标签 ---
    offsetLabel->setAlignment(Qt::AlignCenter);
    offsetLabel->setFont(QFont("Segoe UI", 12, QFont::Medium));

    timeLabel->setFont(QFont("Consolas", 42, QFont::Bold));
    timeLabel->setAlignment(Qt::AlignCenter);

    dateLabel->setFont(QFont("Segoe UI", 16));
    dateLabel->setAlignment(Qt::AlignCenter);

    layout->addLayout(topLayout);
    layout->addWidget(offsetLabel);
    layout->addStretch();
    layout->addWidget(timeLabel);
    layout->addWidget(dateLabel);
    layout->addStretch();

    setCentralWidget(central);

    // --- 信号槽 ---
    connect(timer, &QTimer::timeout, this, &MainWindow::updateTime);
    connect(timeZoneComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onTimeZoneChanged);
    connect(themeButton, &QPushButton::clicked, this, &MainWindow::toggleTheme);
    connect(systemTzButton, &QPushButton::clicked, this, &MainWindow::useSystemTimeZone);
    connect(filterEdit, &QLineEdit::textChanged, this, &MainWindow::filterTimeZones);
    connect(autoThemeCheckBox, &QCheckBox::toggled, this, &MainWindow::onAutoThemeToggled);

    // --- 初始化 ---
    loadSettings();
    applyTheme();
    timer->start(1000);
    updateTime();

    setWindowTitle("🌍 世界时钟");
    resize(900, 400);
}

MainWindow::~MainWindow() {}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QMainWindow::paintEvent(event);

    if (!worldMap.isNull()) {
        QPainter painter(this);
        painter.drawPixmap(rect(), worldMap);
        drawDayNightOverlay(painter);
    }
}

void MainWindow::drawDayNightOverlay(QPainter &painter)
{
    if (worldMap.isNull()) return;

    QDateTime nowUtc = QDateTime::currentDateTimeUtc();
    double utcHour = nowUtc.time().hour() + nowUtc.time().minute() / 60.0;
    double sunLongitude = (utcHour / 24.0) * 360.0 - 180.0; // -180 ~ +180

    int width = worldMap.width();
    QPixmap overlay(worldMap.size());
    overlay.fill(Qt::transparent);

    QPainter overlayPainter(&overlay);
    double sunCenterX = (180 + sunLongitude) / 360.0 * width;

    QLinearGradient grad(sunCenterX, 0, sunCenterX + width / 2.0, 0);
    grad.setColorAt(0.0, QColor(0, 0, 0, 120));
    grad.setColorAt(0.5, QColor(0, 0, 0, 0));
    grad.setColorAt(1.0, QColor(0, 0, 0, 120));

    overlayPainter.fillRect(overlay.rect(), grad);
    overlayPainter.end();

    painter.drawPixmap(rect(), overlay);
}

void MainWindow::loadTimeZones()
{
    QList<QByteArray> zones = QTimeZone::availableTimeZoneIds();
    for (const QByteArray &z : zones)
    {
        //QTimeZone Item = QTimeZone(z) ;
        //allTimeZones << QString::fromUtf8(z) + " " + getUtcOffsetString(Item);
        allTimeZones << QString::fromUtf8(z) ;
    }
    timeZoneComboBox->addItems(allTimeZones);
}

void MainWindow::filterTimeZones(const QString &text)
{
    timeZoneComboBox->clear();
    if (text.isEmpty()) {
        timeZoneComboBox->addItems(allTimeZones);
        return;
    }

    QStringList filtered;
    for (const QString &tz : allTimeZones)
        if (tz.contains(text, Qt::CaseInsensitive))
            filtered << tz;
    timeZoneComboBox->addItems(filtered);

}

void MainWindow::loadSettings()
{
    QString savedZone = settings->value("timezone", QTimeZone::systemTimeZoneId()).toString();
    int index = timeZoneComboBox->findText(savedZone);
    if (index >= 0)
        timeZoneComboBox->setCurrentIndex(index);

    currentZone = QTimeZone(savedZone.toUtf8());
    darkModeEnabled = settings->value("darkMode", false).toBool();
    autoThemeEnabled = settings->value("autoTheme", true).toBool();
    autoThemeCheckBox->setChecked(autoThemeEnabled);
}

void MainWindow::saveSettings()
{
    settings->setValue("timezone", QString::fromUtf8(currentZone.id()));
    settings->setValue("darkMode", darkModeEnabled);
    settings->setValue("autoTheme", autoThemeEnabled);
}

void MainWindow::updateTime()
{
    QDateTime now = QDateTime::currentDateTimeUtc();
    QDateTime local = now.toTimeZone(currentZone);

    timeLabel->setText(local.toString("HH:mm:ss"));
    dateLabel->setText(local.toString("dddd, yyyy-MM-dd"));
    offsetLabel->setText("当前偏移：" + getUtcOffsetString(currentZone));

    autoDetectTheme();
    update(); // 让地图动态刷新
}

void MainWindow::onTimeZoneChanged(int index)
{
    if (index < 0 || index >= timeZoneComboBox->count()) return;
    QByteArray zoneId = timeZoneComboBox->currentText().toUtf8();
    currentZone = QTimeZone(zoneId);
    saveSettings();
    updateTime();
}

void MainWindow::useSystemTimeZone()
{
    QTimeZone sysZone = QTimeZone::systemTimeZone();
    currentZone = sysZone;
    QString zoneId = QString::fromUtf8(sysZone.id());

    int idx = timeZoneComboBox->findText(zoneId);
    if (idx >= 0)
        timeZoneComboBox->setCurrentIndex(idx);
    else {
        timeZoneComboBox->addItem(zoneId);
        timeZoneComboBox->setCurrentText(zoneId);
    }

    saveSettings();
    updateTime();
}

void MainWindow::toggleTheme()
{
    autoThemeEnabled = false; // 手动切换后禁用自动模式
    autoThemeCheckBox->setChecked(false);
    darkModeEnabled = !darkModeEnabled;
    applyTheme();
    saveSettings();
}

void MainWindow::onAutoThemeToggled(bool checked)
{
    autoThemeEnabled = checked;
    saveSettings();
    if (checked) autoDetectTheme();
}

void MainWindow::applyTheme()
{
    QPalette pal;
    if (darkModeEnabled) {
        pal.setColor(QPalette::WindowText, Qt::white);
        pal.setColor(QPalette::Base, QColor(45, 45, 45));
        pal.setColor(QPalette::Text, Qt::white);
        setStyleSheet("QPushButton { background-color:#444; color:white; border-radius:8px; padding:6px; }"
                      "QPushButton:hover { background-color:#666; }"
                      "QLineEdit { background-color:#333; color:white; border-radius:6px; padding:4px; }"
                      "QCheckBox { color:white; }"
                      "QLabel { color:white; }");
        themeButton->setText("☀️ 日间模式");
    } else {
        pal.setColor(QPalette::WindowText, Qt::black);
        pal.setColor(QPalette::Base, QColor(245, 245, 245));
        pal.setColor(QPalette::Text, Qt::black);
        setStyleSheet("QPushButton { background-color:#e0e0e0; color:black; border-radius:8px; padding:6px; }"
                      "QPushButton:hover { background-color:#d0d0d0; }"
                      "QLineEdit { background-color:white; color:black; border-radius:6px; padding:4px; }"
                      "QCheckBox { color:black; }"
                      "QLabel { color:black; }");
        themeButton->setText("🌙 夜间模式");
    }
    setPalette(pal);
}

void MainWindow::autoDetectTheme()
{
    if (!autoThemeEnabled) return;
    QTime now = QTime::currentTime();
    bool shouldBeDark = (now.hour() >= 20 || now.hour() < 6);

    if (shouldBeDark != darkModeEnabled) {
        darkModeEnabled = shouldBeDark;
        applyTheme();
        saveSettings();
    }
}

QString MainWindow::getUtcOffsetString(const QTimeZone &zone)
{
    int offsetSecs = zone.offsetFromUtc(QDateTime::currentDateTimeUtc());
    int hours = offsetSecs / 3600;
    int minutes = (offsetSecs % 3600) / 60;
    QChar sign = (offsetSecs >= 0) ? '+' : '-';
    return QString("(UTC%1%2)")
        .arg(sign)
        .arg(qAbs(hours), 2, 10, QLatin1Char('0'));
        //.arg(qAbs(minutes), 2, 10, QLatin1Char('0'));
}
