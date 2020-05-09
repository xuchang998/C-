#include "mainwindow.h"

#include <QDebug>
#include <iostream>

#include <QMap>
#include <QLinkedList>

#include <QLabel>
#include <QAction>
#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QMenu>
#include <QPushButton>
#include <QEvent>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QListView>
#include <QMessageBox>

#include <QListWidget>
#include <QListWidgetItem>

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QThread>
#include <QThreadPool>

#include <QMutableLinkedListIterator>

#include "data/Constants.h"
#include "data/Controller.h"
#include "data/Channel.h"

#include "misc/LoggingEvent.h"
#include "misc/Settings.h"

#include "models/ControllerModel.h"

#include "views/ConfigurationDialog.h"
#include "views/AddControllerDialog.h"
#include "views/ControllerDetails.h"
#include "views/ControllerList.h"
#include "views/ControllerWidget.h"
#include "views/ControllerDetails.h"
#include "views/TestGraph.h"

#include "task/StationTask.h"
#include "task/NetworkManager.h"
#include "task/DatabaseManager.h"
#include "task/RedisTask.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    controllerModel(new ControllerModel()),
    networkManager(new NetworkManager()),
    jobs(new QMap<QString, QPair<QThread*, QObject*>>()) {

    createMenu();
    createStatusBar();
    createDataThreads();



    widget = new QWidget(this);
    leftLayout = new QVBoxLayout(this);
    windowLayout = new QHBoxLayout(this);
    controllerList = new ControllerList(controllerModel, networkManager, dbManager);

    addControllerButton = new QPushButton("Add Controller");
    addControllerButton->setIcon(QPixmap(":/img/icon/add.png"));
    addControllerButton->setMaximumWidth(controllerList->maximumWidth() + 50);

    QThread* networkThread = new QThread();
    jobs->insert(QString("NetworkTask").arg(jobs->size()),
        QPair<QThread*, QObject*>{ networkThread, networkManager });


    networkManager->moveToThread(networkThread);
    networkThread->start();


    task = new StationTask(networkManager,
                           controllerModel, dbManager);
    connect(task,SIGNAL(redisConnectStatu(bool)),this,SLOT(onRedisConnectStatu(bool)));
    QThread* taskThread = new QThread();
    jobs->insert(QString("StationTask").arg(jobs->size()),
        QPair<QThread*, QObject*>{ taskThread, task });

    connect(task, &StationTask::dataConverted, dbManager, &DatabaseManager::onExecuteQuery);

//    task->moveToThread(taskThread);
//    taskThread->start();

    Settings *settings = Settings::getInstance();

    const Settings::Params* params = settings->getDatabaseSettings();
    task->onStart(params->mRedisHost, params->mRedisAuth);

    connect(controllerList, &ControllerList::controllerConnected, networkManager,
            &NetworkManager::onListenToController);
    connect(controllerModel, &ControllerModel::modelChanged, controllerList, &ControllerList::updateFromModel);

    showGraph = new QPushButton("Refresh Model");
    showGraph->setIcon(QPixmap(":/img/icon/graph.png"));

    leftLayout->addWidget(addControllerButton);
    leftLayout->addWidget(showGraph);
    leftLayout->addWidget(controllerList);

    windowLayout->addLayout(leftLayout);
    controllerList->updateFromModel();

    if (controllerModel->getControllers().begin() ==
            controllerModel->getControllers().end())

        detailsWidget = new ControllerDetails(nullptr,
            networkManager, dbManager, this);
    else
        detailsWidget = new ControllerDetails(*controllerModel->getControllers().begin(),
            networkManager, dbManager, this);
    windowLayout->addWidget(detailsWidget);
    widget->setLayout(windowLayout);

    setCentralWidget(widget);

    connect(controllerList, &ControllerList::controllerSelected, this, &MainWindow::onControllerSelected);
    connect(addControllerButton, &QPushButton::pressed, this, &MainWindow::onAddControllerPressed);
    connect(controllerList, &ControllerList::modelEmpty, detailsWidget, &ControllerDetails::onModelEmpty);
    connect(this, &MainWindow::logEmitted, detailsWidget, &ControllerDetails::onLogEmmitted);






}

MainWindow::~MainWindow() {
    for (auto& job : *jobs) {
        QThread* thread = job.first;

        thread->requestInterruption();
        thread->wait(1000);
        thread->terminate();

        delete job.second;
    }

    jobs->clear();
}

void MainWindow::createMenu() {
    fileMenu = menuBar()->addMenu(tr("&File"));

    configurationMenu = menuBar()->addMenu(tr("&Configuration"));
    configurationAction = new QAction(tr("Configuration"));
    configurationMenu->addAction(configurationAction);
    helpMenu = menuBar()->addMenu(tr("&Help"));

    connect(configurationAction, &QAction::triggered, this, &MainWindow::onConfigurationClicked);
}

void MainWindow::onDbConnectionChanged(bool connected) {

    if (connected) {
        dbConnectionLabel->setText(tr("DB Connections: %1")
            .arg(dbManager->getConnections()));
        onReadSettings();
    }
}

void MainWindow::onSavedClicked(bool withFile) {
    if (withFile) {
//        controllerModel->clearModel();
        onReadSettings();
    } else {
        controllerModel->modelChanged();
    }

    if (*controllerModel->getControllers().begin() !=
            *controllerModel->getControllers().end())
        detailsWidget->changeController(*controllerModel->getControllers().begin());
    else
        detailsWidget->changeController(nullptr);
}

void MainWindow::onConfigurationClicked() {
    ConfigurationDialog dialog(dbManager);
    connect(&dialog, &ConfigurationDialog::saveClicked, this, &MainWindow::onSavedClicked);
    if (dialog.exec()) { }
    disconnect(&dialog, &ConfigurationDialog::saveClicked, this, &MainWindow::onSavedClicked);
}

void MainWindow::createStatusBar() {
    dbConnectionLabel = new QLabel(" ");
    dbConnectionLabel->setAlignment(Qt::AlignHCenter);

    statusBar()->addWidget(dbConnectionLabel);
}

bool MainWindow::event(QEvent *event) {
    if (static_cast<int>(event->type()) == LOGGING_EVENT_NUMBER) {
        LoggingEvent *logEvent = reinterpret_cast<LoggingEvent *>(event);
        emit logEmitted(logEvent->data);
    }

    return QMainWindow::event(event);
}

void MainWindow::onControllerSelected(Controller *controller) {
    detailsWidget->changeController(controller);
}

void MainWindow::onAddControllerPressed() {
    if (nullptr == networkManager || nullptr == dbManager) return;
    AddControllerDialog addControllerDialog {
        networkManager->getAvailableDevices(), dbManager->getMachines()
    };

    // TODO: Connect accepted signal to onInsertController
    if (addControllerDialog.exec()) {
        Controller* controller = addControllerDialog.getController();
        QFutureWatcher<bool>* watcher = new QFutureWatcher<bool>();
        dbManager->onInsertController(*watcher, controller);

        connect(watcher, &QFutureWatcher<bool>::finished, [&, this, controller, watcher]() {
           if (watcher->result()) {
               controllerModel->onAddController(controller);
               controllerList->updateFromModel();
               Settings::getInstance()->onSaveController(controller);
           } else
               controller->deleteLater();
           watcher->deleteLater();
        });
    }
}

bool MainWindow::onReadSettings() {
    QFutureWatcher<QVector<Controller*>>* watcher = new QFutureWatcher<QVector<Controller*>>();
    dbManager->onGetAllControllers(*watcher);

    connect(watcher, &QFutureWatcher<QVector<Controller*>>::finished, [&, this, watcher]() {
        QVector<Controller*> controllers = watcher->result();
        for (auto& controller : controllers) {
            controllerModel->onAddController(controller);
            Settings::getInstance()->onCompleteController(controller);
            controller->setChannelsBufferSize(controller->getBufferSize());
            emit controllerModel->modelChanged();
        }

        watcher->deleteLater();
    });

    return true;
}

void MainWindow::onRedisConnectStatu(bool connect)
{
    if(!connect)
        dbConnectionLabel->setText("redis connect failed!");

}

void MainWindow::createDataThreads() {
    Settings *settings = Settings::getInstance();
    const Settings::Params* params = settings->getDatabaseSettings();

    if (params->isValid()) {

        dbManager = new DatabaseManager(params->mHostname,
            params->mDatabase, params->mUsername,
            params->mPassword, params->mPort,
            params->mMaxConnections);

        QThread* databaseThread = new QThread();

//        connect(dbManager, &DatabaseManager::databaseConnected,
//            this, &MainWindow::onDbConnectionChanged);

        if(!dbManager->isConnected())
            dbConnectionLabel->setText("mysql connect failed!");


        jobs->insert("Database", QPair<QThread*, QObject*> {
            databaseThread, dbManager });

//        dbManager->moveToThread(databaseThread);
//        databaseThread->start();

        if (dbManager->isConnected())
            onReadSettings();

//        RedisTask* redisTask = new RedisTask(controllerModel, params->mRedisHost);
//        QThread* redisThread = new QThread();
//        redisTask->moveToThread(redisThread);
//        redisThread->start();

//        jobs->insert("Redis", QPair<QThread*, QObject*> {
//            redisThread, redisTask });
    }
    else {
      qDebug() << "params is not valid!";
    }
}
