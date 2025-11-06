#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <Poco/String.h>
#include "Poco/LocalDateTime.h"
#include "Poco/DateTime.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeParser.h"
#include <Poco/Timespan.h>

#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/StreamCopier.h>
#include <Poco/Net/NetworkInterface.h>
#include <Poco/Net/IPAddress.h>

#include <iostream>
#include <vector>

#include <QDebug>

using namespace Poco::Net;
using namespace std;

QStringList getLocalIPAddresses()
{
    QStringList ipList;

    // 遍历所有网络接口
    auto ifList = NetworkInterface::list();
    for (const auto& iface : ifList)
    {
        // 跳过未启用或回环接口
        if (!iface.isUp() || iface.isLoopback())
            continue;

        // 获取接口地址列表
        auto addr = iface.broadcastAddress(0); // Poco 1.14 地址获取方式可能改为 iface.interfaceAddresses()

        ipList.append(QString::fromStdString(addr.toString()));
    }

    return ipList;
}
QString getFirstLocalIPAddress()
{
    QStringList ips = getLocalIPAddresses();
    if (!ips.isEmpty())
        return ips.first();
    return "0.0.0.0";
}


void tcpServer() {
    Poco::Net::SocketAddress sa("0.0.0.0", 12345);
    Poco::Net::ServerSocket server(sa);

    std::cout << "TCP server started on port 12345" << std::endl;

    while (true) {
        Poco::Net::StreamSocket client = server.acceptConnection();
        std::cout << "Client connected: "
                  << client.peerAddress().toString() << std::endl;

        try {
            char buffer[256] = { 0 };
            int n = client.receiveBytes(buffer, sizeof(buffer));
            std::cout << "Received: " << buffer << std::endl;

            const char* response = "Hello from server!";
            client.sendBytes(response, strlen(response));
        }
        catch (Poco::Exception& e) {
            std::cerr << "Error: " << e.displayText() << std::endl;
        }

        client.close();
    }
}


void stringExample() {
    std::string str = "  Hello, Poco!  ";

    // 去除前后空格
    std::string trimmed = Poco::trim(str);
    std::cout << "Trimmed: '" << trimmed << "'" << std::endl;

    // 转换为大写
    std::string upper = Poco::toUpper(trimmed);
    std::cout << "Upper: " << upper << std::endl;

    // 转换为小写
    std::string lower = Poco::toLower(trimmed);
    std::cout << "Lower: " << lower << std::endl;

    // 字符串分割
    std::vector<std::string> parts;
    //Poco::split("one,two,three", ',', parts);
    std::cout << "Split result: ";
    for (const auto& part : parts) {
        std::cout << part << " ";
    }
    std::cout << std::endl;
}

void datetimeExample() {
    // 当前日期时间
    Poco::DateTime now;
    std::cout << "Current datetime: "
              << Poco::DateTimeFormatter::format(now, "%Y-%m-%d %H:%M:%S")
              << std::endl;

    // 解析日期时间
    std::string str = "2023-05-15 14:30:00";
    Poco::DateTime dt;
    int tz;
    Poco::DateTimeParser::parse("%Y-%m-%d %H:%M:%S", str, dt, tz);
    std::cout << "Parsed datetime: "
              << Poco::DateTimeFormatter::format(dt, "%Y-%m-%d %H:%M:%S")
              << std::endl;

    // 时间差计算
    Poco::DateTime later = now + Poco::Timespan(2, 0, 0, 0, 0); // 加2天
    std::cout << "Two days later: "
              << Poco::DateTimeFormatter::format(later, "%Y-%m-%d")
              << std::endl;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    resize(400,300);

    stringExample();
    datetimeExample();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    //tcpServer() ;
    //tcpClient();

    QString str = getFirstLocalIPAddress() ;

    ui->label_Ip->setText(str);
}

