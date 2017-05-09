#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include "network.h"
#include <QCloseEvent>

// Enseñar aplicacion al profesor
// Preguntar si esto serviria como tarea
// Preguntar Parent
// Ver como implementar openCV en forma de objeto


extern Dialog *d;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui -> pushButtonShoot -> setStyleSheet("background-color: rgba(255,0,0,0.6);  border-style: outset;");
    ui -> pushButtonArmed -> setStyleSheet("background-color: rgba(255,0,0,0.6);  border-style: outset;");
    EnableDisableMainMenu(0);

    server = new CServer(this);
    client = new CClient(this);

    connect(server                                , SIGNAL(newConnection()), this, SLOT(ServerNewConnection()));

    connect(ui->SliderX, SIGNAL(valueChanged(int)), this, SLOT(SliderXSend()));
    connect(ui->SliderY, SIGNAL(valueChanged(int)), this, SLOT(SliderYSend()));
    connect(ui->pushButtonArmed, SIGNAL(clicked(bool)), this, SLOT(ArmedSendClicked()));
    connect(ui->pushButtonShoot, SIGNAL(pressed()), this, SLOT(ShootSendOne()));
    connect(ui->pushButtonShoot, SIGNAL(released()),this,SLOT(ShootSendZero()));

    connect(client, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(ClientStateChanged(QAbstractSocket::SocketState)));
    connect(client, SIGNAL(hostFound()),this, SLOT(ClientHostFound()));
    connect(client, SIGNAL(connected()),this, SLOT(ClientConnected()));
    connect(client, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(ClientError(QAbstractSocket::SocketError)));
    connect(client, SIGNAL(readyRead()), this, SLOT(ClientRead()));
}

MainWindow::~MainWindow()
{
    delete ui;

    delete server;
    delete client;
}

void MainWindow::ServerActivateDeactivate(void)
{
    if(server->isListening())
    {
        d -> EditPlainTextServer(QString("User: Deactivate") + '\n');
        server->close();
    }
    else
    {
        d -> EditPlainTextServer(QString("User: Activate "));

        if(server->listen(QHostAddress::Any, d->getLineEditServerPort().toInt()))
            d -> EditPlainTextServer(QString("OK\n"));          
        else
            d -> EditPlainTextServer(QString("NO OK\n"));            
    }
}

void MainWindow::ServerNewConnection(void)
{
    d->EditPlainTextServer(QString("Signal: newConnection()") + '\n');
}

void MainWindow::ServerRead(void)
{

}


void MainWindow::ClientConnectDisconnect(void)
{
    if(client->isOpen())
    {
        d ->EditPlainTextClient(QString("User: Disconnect") + '\n');

        d -> LabelClientLocalAddress(QString("Local Address = ?"));
        d -> LabelClientLocalPort(QString("Local Port = ?"));
        client->close();
    }

    else
    {
        d -> EditPlainTextClient(QString("User: Connect") + '\n');

        client -> connectToHost(d -> getLineEditClientRemoteAdress(), d-> getLineEditClientRemotePort().toInt());
        }
}

void MainWindow::ClientStateChanged(QAbstractSocket::SocketState state)
{
    QString state_text("Signal: stateChanged( ");
    switch (state)
    {
        case QAbstractSocket::UnconnectedState:
            state_text.append("UnconnectedState )");
            break;
        case QAbstractSocket::HostLookupState:
            state_text.append("HostLookupState )");
            break;
        case QAbstractSocket::ConnectingState:
            state_text.append("ConnectingState )");
            break;
        case QAbstractSocket::ConnectedState:
            state_text.append("ConnectedState )");
            break;
        case QAbstractSocket::BoundState:
            state_text.append("BoundState )");
            break;
        case QAbstractSocket::ClosingState:
            state_text.append("ClosingState )");
            break;
        case QAbstractSocket::ListeningState:
            state_text.append("ListeningState )");
            break;
    }
    d -> EditPlainTextClient(state_text + '\n');
}

void MainWindow::ClientHostFound(void)
{
    d -> EditPlainTextClient(QString("Signal: hostFound()") + '\n');
}

void MainWindow::ClientConnected(void)
{
    d -> EditPlainTextClient(QString("Signal: connected()") + '\n');

    d -> LabelClientLocalAddress(QString("Local Address = ") + client->localAddress().toString());
    d -> LabelClientLocalPort(QString("Local Port = ")    + QString::number(client->localPort()));

    d -> LineEditClientRemoteAddress(QString(client->peerAddress().toString()));
    d -> LineEditClientRemotePort(QString::number(client->peerPort()));
}

void MainWindow::ClientError(QAbstractSocket::SocketError error)
{
    d -> EditPlainTextClient(QString("Signal: error( ") + client->errorString() + " )\n");
}


bool MainWindow::SliderXSend(void)
{
    if(d->getStateClient()) //Siempre que esta máquina actue como maquina remota
    {
        QString text = "X" + QString::number(ui -> SliderX -> value());
        QByteArray data = text.toUtf8();
        return client->write(data) == data.size();
    }
    return 0;
}

bool MainWindow::SliderYSend(void)
{
    if(d->getStateClient()) //Siempre que esta máquina actue como maquina remota
    {
        QString text = "Y" + QString::number(ui -> SliderY -> value());
        QByteArray data = text.toUtf8();
        return client->write(data) == data.size();
    }
    return 0;
}


bool MainWindow::ShootSendZero()
{
    if(d->getStateClient()) //Siempre que esta máquina actue como maquina remota
    {
        ui -> pushButtonShoot -> setStyleSheet("background-color: rgba(255,0,0,0.6);  border-style: outset;");
        QString text = "S0";
        QByteArray data = text.toUtf8();
        return client->write(data) == data.size();
    }
    return 0;
}


bool MainWindow::ShootSendOne()
{
    if(d->getStateClient()) //Siempre que esta máquina actue como maquina remota
    {
        ui -> pushButtonShoot -> setStyleSheet("background-color: rgba(0,255,0,0.6);  border-style: outset;");
        QString text = "S1";
        QByteArray data = text.toUtf8();
        return client->write(data) == data.size();
    }
    return 0;
}

bool MainWindow::ArmedSendClicked()
{
    if(d->getStateClient()) //Siempre que esta máquina actue como maquina remota
    {
        QString text = ui -> pushButtonArmed -> text();

        if(text == "ARMAR")
        {
            ui -> pushButtonArmed -> setStyleSheet("background-color: rgba(0,255,0,0.6);  border-style: outset;");
            ui -> pushButtonArmed -> setText("ARMADO");
            QString text = "A1";
            QByteArray data = text.toUtf8();
            return client->write(data) == data.size();
        }
        else
        {
            ui -> pushButtonArmed -> setStyleSheet("background-color: rgba(255,0,0,0.6);  border-style: outset;");
            ui -> pushButtonArmed -> setText("ARMAR");
            QString text = "A0";
            QByteArray data = text.toUtf8();
            return client->write(data) == data.size();
        }
    }
    return 0;
}




void MainWindow::ClientRead(void)
{
    QString read = client ->readAll(); // Solo se puede leer una única vez, ya que una vez leido el valor de readAll se borra

    d -> EditPlainTextClient(QString("Read: ") + read + '\n');
}


void MainWindow::on_pushButtonSetting_clicked()
{
    d->open();
}


void MainWindow::LabelInformationServerClient(QString mode)
{
    if(mode == "server")
        ui -> labelInformationServerClient -> setText("Modo: SERVIDOR");
    else if (mode == "client")
        ui -> labelInformationServerClient -> setText("Modo: CLIENTE");
    else if (mode == "nothing")
        ui -> labelInformationServerClient -> setText("Modo: sin conexion");

}

void MainWindow::EnableDisableMainMenu(bool state)
{
    ui -> SliderX -> setEnabled(state);
    ui -> SliderY -> setEnabled(state);
    ui -> pushButtonArmed -> setEnabled(state);
    ui -> pushButtonShoot -> setEnabled(state);

}

void MainWindow::setSliderX(int value)
{
    ui->SliderX -> setValue(value);
}

void MainWindow::setSliderY(int value)
{
    ui->SliderY -> setValue(value);
}


void MainWindow::closeEvent (QCloseEvent *event)
{
    event -> accept(); // Se acepta el evento
    d->close(); // Se cierra la ventana "Configuracion IP"
}
