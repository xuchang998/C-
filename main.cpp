#include "mainwindow.h"
#include <QApplication>

#include <iostream>

#include "qredisclient/redisclient.h"
#include "misc/LogUtils.h"

int main(int argc, char *argv[]) {
        QApplication a(argc, argv);
    initRedisClient();

    LOGUTILS::initLogging();

    MainWindow w;
    LOGUTILS::setWidget(&w);
    w.show();

//    RedisClient::ConnectionConfig cfg("127.0.0.1", "yxck@#8888");
//    RedisClient::Connection connction(cfg);
//    std::cout << connction.connect(true) << std::endl;
//    RedisClient::Response response = connction.commandSync({"KEYS", "*_18_*"});

//    std::cout << "Printing......" << std::endl;
//    for (auto& b : response.value().toStringList())
//        std::cout << b.toStdString() << std::endl;

    return a.exec();
//    return 0;
}
