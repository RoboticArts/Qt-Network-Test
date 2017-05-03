#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"

extern Dialog *d;

//////////////////////////////////////////////////////////////////////////////////////////

CConnection::CConnection(QObject *parent) :
    QTcpSocket(parent)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(Read()));
}

bool CConnection::SendMessage(const QString &message)
{
    if(message.isEmpty())
        return false;

    QByteArray msg = message.toUtf8();
    QByteArray data = "MESSAGE " + QByteArray::number(msg.size()) + ' ' + msg;
    return write(data) == data.size();
}

void CConnection::Read(void)  //Lectura del Servidor cuando el cliente manda un dato
{
    QString read = readAll();

    //((MainWindow*)(parent()->parent()))->ui->plainTextEditServer->insertPlainText(QString("Read: ") + read + '\n');

    //Se descompone el prefijo X del numero: X16: se obtiene X y 16

    if(read.startsWith("X"))
        ((MainWindow*)(parent()->parent()))->ui->SliderControladoX -> setValue(read.right(2).toInt());
        //d -> EditPlainTextServer(read.right(2));

    if(read.startsWith("Y"))
        ((MainWindow*)(parent()->parent()))->ui->SliderControladoY -> setValue(read.right(2).toInt());
        //d -> EditPlainTextServer(read.right(2));

    d -> EditPlainTextServer(QString("Read: ") + read + '\n');
}

CClient::CClient(QObject *parent) :
    QTcpSocket(parent)
{

}

CServer::CServer(QObject *parent) :
    QTcpServer(parent)
{

}

void CServer::incomingConnection(qintptr socketDescriptor)
{
    CConnection *connection = new CConnection(this);
    connection->setSocketDescriptor(socketDescriptor);
    emit newConnection(connection);
}

//////////////////////////////////////////////////////////////////////////////////////////



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    server = new CServer(this);
    client = new CClient(this);

    //connect(ui->pushButtonServerActivateDeactivate, SIGNAL(clicked(bool))  , this, SLOT(ServerActivateDeactivate()));
    connect(ui->pushButtonServerSend              , SIGNAL(clicked(bool))  , this, SLOT(ServerSend()));
    connect(server                                , SIGNAL(newConnection()), this, SLOT(ServerNewConnection()));

    //connect(ui->SliderX, SIGNAL(sliderReleased()), this, SLOT(ServerNewValue()));
    connect(ui->SliderX, SIGNAL(valueChanged(int)), this, SLOT(SliderXSend()));
    connect(ui->SliderY, SIGNAL(valueChanged(int)), this, SLOT(SliderYSend()));

    //connect(ui->pushButtonClientConnectDisconnect, SIGNAL(clicked(bool)), this, SLOT(ClientConnectDisconnect()));
    connect(ui->pushButtonClientSend             , SIGNAL(clicked(bool)), this, SLOT(ClientSend()));
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
        //ui->pushButtonServerActivateDeactivate->setText(QString("Activate"));
        //ui->plainTextEditServer->insertPlainText(QString("User: Deactivate") + '\n');

        d -> EditPlainTextServer(QString("User: Deactivate") + '\n');
        server->close();
    }
    else
    {
        //ui->pushButtonServerActivateDeactivate->setText(QString("Deactivate"));
        //ui->plainTextEditServer->insertPlainText(QString("User: Activate "));

        d -> EditPlainTextServer(QString("User: Activate "));

        //if(server->listen(QHostAddress::Any, ui->lineEditServerPort->text().toInt())){
        if(server->listen(QHostAddress::Any, d->getLineEditServerPort().toInt())){

            //ui->plainTextEditServer->insertPlainText(QString("OK\n"));
            d -> EditPlainTextServer(QString("OK\n"));
            }

        else{
            //ui->plainTextEditServer->insertPlainText(QString("NO OK\n"));
            d -> EditPlainTextServer(QString("NO OK\n"));
            }
    }
}

void MainWindow::ServerNewConnection(void)
{
    //ui->plainTextEditServer->insertPlainText(QString("Signal: newConnection()") + '\n');
    d->EditPlainTextServer(QString("Signal: newConnection()") + '\n');

}

void MainWindow::ServerRead(void)
{

}

void MainWindow::ServerSend(void)
{
    //ui->plainTextEditServer->insertPlainText(QString("User: Send\nwrite( ") + ui->lineEditServerMessageToSend->text() + " )\n");
    d -> EditPlainTextServer(QString("User: Send\nwrite( ") + ui->lineEditServerMessageToSend->text() + " )\n");

    QByteArray data = ui->lineEditServerMessageToSend->text().toUtf8();
    QList<QObject*> connections = server->children();
    foreach (QObject *connection, connections)
        ((CConnection*)connection)->write(data);
}


///////// NO SE EMPLEARÁ
void MainWindow::ServerNewValue(void)
{
    QString message(QString("value = ") + QString::number(ui->SliderX->value()));
    ui->labelValue->setText(message);

    //ui->plainTextEditServer->insertPlainText(QString("write( ") + message + " )\n");
    d -> EditPlainTextServer(QString("write( ") + message + " )\n");

    QByteArray data = message.toUtf8();
    QList<QObject*> connections = server->children();
    foreach (QObject *connection, connections)
        ((CConnection*)connection)->write(data);
}


void MainWindow::ClientConnectDisconnect(void)
{
    if(client->isOpen())
    {
        //ui->pushButtonClientConnectDisconnect->setText(QString("Connect"));

        //ui->plainTextEditClient->insertPlainText(QString("User: Disconnect") + '\n');
        d ->EditPlainTextClient(QString("User: Disconnect") + '\n');

        d -> LabelClientLocalAddress(QString("Local Address = ?"));
        d -> LabelClientLocalPort(QString("Local Port = ?"));
       // ui->labelClientLocalAddress->setText(QString("Local Address = ?"));
       // ui->labelClientLocalPort   ->setText(QString("Local Port = ?"));
        client->close();
    }
    else
    {
        //ui->pushButtonClientConnectDisconnect->setText(QString("Disconnect"));

        //ui->plainTextEditClient->insertPlainText(QString("User: Connect") + '\n');
        d -> EditPlainTextClient(QString("User: Connect") + '\n');

        client -> connectToHost(d -> getLineEditClientRemoteAdress(),
                                d-> getLineEditClientRemotePort().toInt());
        /*
        client->connectToHost(ui->lineEditClientRemoteAddress->text(),
                              ui->lineEditClientRemotePort->text().toInt());
        */
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
    //ui->plainTextEditClient->insertPlainText(state_text + '\n');
    d -> EditPlainTextClient(state_text + '\n');
}

void MainWindow::ClientHostFound(void)
{
    //ui->plainTextEditClient->insertPlainText(QString("Signal: hostFound()") + '\n');
    d -> EditPlainTextClient(QString("Signal: hostFound()") + '\n');
}

void MainWindow::ClientConnected(void)
{
    //ui->plainTextEditClient->insertPlainText(QString("Signal: connected()") + '\n');
    d -> EditPlainTextClient(QString("Signal: connected()") + '\n');

    d -> LabelClientLocalAddress(QString("Local Address = ") + client->localAddress().toString());
    d -> LabelClientLocalPort(QString("Local Port = ")    + QString::number(client->localPort()));
   // ui->labelClientLocalAddress->setText(QString("Local Address = ") + client->localAddress().toString());
   // ui->labelClientLocalPort   ->setText(QString("Local Port = ")    + QString::number(client->localPort()));

    d -> LineEditClientRemoteAddress(QString(client->peerAddress().toString()));
    d -> LineEditClientRemotePort(QString::number(client->peerPort()));
   // ui->lineEditClientRemoteAddress->setText(QString(client->peerAddress().toString()));
   // ui->lineEditClientRemotePort   ->setText(QString::number(client->peerPort()));
}

void MainWindow::ClientError(QAbstractSocket::SocketError error)
{
    //ui->plainTextEditClient->insertPlainText(QString("Signal: error( ") + client->errorString() + " )\n");
    d -> EditPlainTextClient(QString("Signal: error( ") + client->errorString() + " )\n");
}

bool MainWindow::ClientSend(void)
{
    QByteArray data = ui->lineEditClientMessageToSend->text().toUtf8();
    return client->write(data) == data.size();
}

bool MainWindow::SliderXSend(void)
{

    QString text = "X" + QString::number(ui -> SliderX -> value());
    QByteArray data = text.toUtf8();
    return client->write(data) == data.size();
}

bool MainWindow::SliderYSend(void)
{

    QString text = "Y" + QString::number(ui -> SliderY -> value());
    QByteArray data = text.toUtf8();
    return client->write(data) == data.size();
}

void MainWindow::ClientRead(void)
{
    QString read = client ->readAll(); // Solo se puede leer una única vez, ya que una vez leido el valor de readAll se borra

    //ui->plainTextEditClient->insertPlainText(QString("Read: ") + read + '\n');
    d -> EditPlainTextClient(QString("Read: ") + read + '\n');
}


void MainWindow::on_pushButtonSetting_clicked()
{
    d->open();
}


void MainWindow::setSliderControladoX(int value)
{
    ui -> SliderControladoX -> setValue(value);
}

void MainWindow::setSliderControladoY(int value)
{
    ui -> SliderControladoY -> setValue(value);
}
