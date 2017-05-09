#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>

#include <network.h>

namespace Ui {
class MainWindow;
}



class MainWindow : public QMainWindow
{
    Q_OBJECT

    CServer *server;
    CClient *client;

public:
    explicit MainWindow(QWidget *parent = 0);

    void LabelInformationServerClient(QString mode);
    void EnableDisableMainMenu(bool state);
    void closeEvent (QCloseEvent *event);

    ~MainWindow();

private:
    Ui::MainWindow *ui;

public slots:
    void ServerActivateDeactivate(void);
    void ServerNewConnection(void);
    void ServerRead(void);


    void ClientConnectDisconnect(void);
    void ClientStateChanged(QAbstractSocket::SocketState state);
    void ClientHostFound(void);
    void ClientConnected(void);
    void ClientError(QAbstractSocket::SocketError error);

    void ClientRead(void);

    bool SliderXSend(void);
    bool SliderYSend(void);

    void setSliderX(int value);
    void setSliderY(int value);

    bool ShootSendZero(void);
    bool ShootSendOne(void);

    bool ArmedSendClicked(void);

    void on_pushButtonSetting_clicked(); //Boton "configuracion"



private slots:

};

#endif // MAINWINDOW_H
