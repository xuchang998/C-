#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLabel;
class QMenu;
class QPushButton;
class QToolBar;
class QAction;
class QPushButton;
class QHBoxLayout;
class QVBoxLayout;
class QSplitter;
class QEvent;
class QThread;
class QThreadPool;

class DatabaseManager;

class StationTask;
class Controller;
class ControllerModel;
class ControllerList;
class ControllerDetails;

class NetworkManager;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool event(QEvent* event) override;

signals:
    void logEmitted(QString& log);


public slots:
    void onDbConnectionChanged(bool isConnected);
    void onControllerSelected(Controller* controller);
    void onAddControllerPressed();
    void onConfigurationClicked();
    void onSavedClicked(bool withFile);
//    void onConnectController(const Controller* controller, bool isConnect);
    bool onReadSettings(void);

//    void onTest(QString string, QList<QVariant> values);

    void onRedisConnectStatu(bool);

private:
    void createMenu(void);
    void createStatusBar(void);
    void createSettingsThread();
    void createDataThreads();

private:

    QHBoxLayout* windowLayout;
    ControllerList* controllerList;
    ControllerDetails* detailsWidget;

    QVBoxLayout* leftLayout;
    QPushButton* addControllerButton;
    QPushButton* showGraph;

    QMenu* fileMenu;
    QMenu* configurationMenu;
    QMenu* helpMenu;

    QAction* configurationAction;

    QLabel* dbConnectionLabel;
    QLabel* dbConnStatusLabel;
    bool redisConnection = false;
    bool mysqlConnection = false;
    QLabel* dmConnectionLabel;
    QLabel* dmConnStatusLabel;

    QWidget* widget;

    ControllerModel* controllerModel;
    DatabaseManager* dbManager;
    NetworkManager* networkManager;

    QMap<QString, QPair<QThread*, QObject*>>* jobs;
    //QLinkedList<StationTask*>* tasks;

    StationTask* task;

    unsigned short dbConnectionsCounter = 0;
    bool isConfigurationDB = false;
    bool isConfigurationFile = false;
};

#endif // MAINWINDOW_H
