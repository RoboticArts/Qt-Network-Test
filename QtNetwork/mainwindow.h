#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>

namespace Ui {
class MainWindow;
}

class CConnection : public QTcpSocket
{
    Q_OBJECT

public:
    CConnection(QObject *parent = 0);
    bool SendMessage(const QString &message);

public slots:
    void Read(void);
};




class CClient : public QTcpSocket
{
    Q_OBJECT

public:
    CClient(QObject *parent = 0);
};





class CServer : public QTcpServer
{
    Q_OBJECT

public:
    CServer(QObject *parent = 0);

signals:
    void newConnection(CConnection *connection);

protected:
    void incomingConnection(qintptr socketDescriptor) Q_DECL_OVERRIDE;
};





class MainWindow : public QMainWindow
{
    Q_OBJECT

    CServer *server;
    CClient *client;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

//private:
    Ui::MainWindow *ui;

public slots:
    void ServerActivateDeactivate(void);
    void ServerNewConnection(void);
    void ServerRead(void);
    void ServerSend(void);
    void ServerNewValue(void);

    void ClientConnectDisconnect(void);
    void ClientStateChanged(QAbstractSocket::SocketState state);
    void ClientHostFound(void);
    void ClientConnected(void);
    void ClientError(QAbstractSocket::SocketError error);

    bool ClientSend(void);
    void ClientRead(void);

    bool SliderXSend(void);
    bool SliderYSend(void);

    void setSliderControladoX(int value);
    void setSliderControladoY(int value);

private slots:
    void on_pushButtonSetting_clicked();
};

#endif // MAINWINDOW_H
