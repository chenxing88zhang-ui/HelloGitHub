#ifndef TIMEZONESWICHER_H
#define TIMEZONESWICHER_H

#include <QWidget>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QDateTime>
#include <QTimeZone>

class TimeZoneSwicher : public QWidget
{
    Q_OBJECT

public:
    TimeZoneSwicher(QWidget *parent = nullptr);
    ~TimeZoneSwicher();

private slots:
    void updateTime(const QString &zoneIdStr);

private:
    QComboBox *timezoneComboBox;
    QLabel *label;
};
#endif // TIMEZONESWICHER_H
