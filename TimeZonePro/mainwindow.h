#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QLabel>
#include <QTimer>
#include <QTimeZone>
#include <QSettings>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QPixmap>
#include <QStringList>
#include <QMap>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateTime();
    void onTimeZoneChanged(int index);
    void toggleTheme();
    void filterTimeZones(const QString &text);
    void useSystemTimeZone();
    void onAutoThemeToggled(bool checked);

private:
    // UI 控件
    QComboBox *timeZoneComboBox;
    QLineEdit *filterEdit;
    QLabel *timeLabel;
    QLabel *dateLabel;
    QLabel *offsetLabel;
    QPushButton *themeButton;
    QPushButton *systemTzButton;
    QCheckBox *autoThemeCheckBox;

    // 状态管理
    QTimer *timer;
    QSettings *settings;
    QTimeZone currentZone;
    bool darkModeEnabled;
    bool autoThemeEnabled;
    QStringList allTimeZones;

    // 背景地图
    QPixmap worldMap;

    // 内部方法
    void loadTimeZones();
    void loadSettings();
    void saveSettings();
    void applyTheme();
    void autoDetectTheme();
    QString getUtcOffsetString(const QTimeZone &zone);
    void drawDayNightOverlay(QPainter &painter);

    QMap<QString, QPointF> timeZoneCoordinates;  // 时区经纬度表
};

#endif // MAINWINDOW_H
