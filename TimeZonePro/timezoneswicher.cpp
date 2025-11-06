#include "timezoneswicher.h"

TimeZoneSwicher::TimeZoneSwicher(QWidget *parent)
    : QWidget(parent)
{
    auto layout = new QVBoxLayout(this);

    // 下拉框选择时区
    timezoneComboBox = new QComboBox(this);
    label = new QLabel(this);

    // 获取所有可用的时区
    auto timeZones = QTimeZone::availableTimeZoneIds();
    for (const QByteArray &zoneId : timeZones) {
        timezoneComboBox->addItem(QString::fromUtf8(zoneId));
    }

    // 连接信号与槽
    connect(timezoneComboBox, &QComboBox::currentTextChanged, this, &TimeZoneSwicher::updateTime);

    layout->addWidget(timezoneComboBox);
    layout->addWidget(label);

    // 初始化显示第一个时区时间
    updateTime(timezoneComboBox->currentText());

}

TimeZoneSwicher::~TimeZoneSwicher()
{
}

void TimeZoneSwicher::updateTime(const QString &zoneIdStr)
{
    QTimeZone timeZone(zoneIdStr.toUtf8());
    if (!timeZone.isValid()) {
        label->setText("Invalid Time Zone");
        return;
    }

    // 获取当前UTC时间
    QDateTime utcTime = QDateTime::currentDateTimeUtc();

    // 转换到选中的时区
    QDateTime localTime = utcTime.toTimeZone(timeZone);

    // 格式化时间显示
    QString timeStr = localTime.toString("yyyy-MM-dd HH:mm:ss");
    label->setText(QString("Time in %1: %2").arg(zoneIdStr, timeStr));

}

