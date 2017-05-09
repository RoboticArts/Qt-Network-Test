#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include <QCloseEvent>


extern Dialog *d;

//////////////////////////////////////////////////////////////////////////////////////////

// Cerrar ventana
// Inhabilitar sliders
// Corregir cuadro remote adress

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
    QString read = readAll(); //Se modifica
    QString auxRead = read; // Copia de seguridad

    //((MainWindow*)(parent()->parent()))->ui->plainTextEditServer->insertPlainText(QString("Read: ") + read + '\n');

    //Read es un buffer. Se halla la ultima X y se trunca el valor. X14X56X35 se obtiene 35 (el ultimo valor)

    if(read.startsWith("X"))
    {
        int n = read.lastIndexOf("X");
        read =  read.remove(0,n+1);
        ((MainWindow*)(parent()->parent()))->ui->SliderX -> setValue(read.toInt());

    }

    if(read.startsWith("Y"))
    {
        int n = read.lastIndexOf("Y"); //Se posiciona en la ultima Y recibida
        read =  read.remove(0,n+1); // y borra todo lo anterior, incluido la Y. Se queda el número
        ((MainWindow*)(parent()->parent()))->ui->SliderY -> setValue(read.toInt());

    }

    if(read.startsWith("S"))
    {
        int n = read.lastIndexOf("S");
        read = read.remove(0,n+1);
        // d -> EditPlainTextServer(read.remove(0,1));
    }

    if(read.startsWith("A"))
    {
        int n = read.lastIndexOf("A");
        read = read.remove(0,n+1);
        // d -> EditPlainTextServer(read.remove(0,1));
    }


    d -> EditPlainTextServer(QString("Read(") + auxRead.mid(0,1) + QString("): ")  + read + '\n');
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

    ui -> pushButtonShoot -> setStyleSheet("background-color: rgba(255,0,0,0.6);  border-style: outset;");
    ui -> pushButtonArmed -> setStyleSheet("background-color: rgba(255,0,0,0.6);  border-style: outset;");
    EnableDisableMainMenu(0);

    server = new CServer(this);
    client = new CClient(this);

    //connect(ui->pushButtonServerActivateDeactivate, SIGNAL(clicked(bool))  , this, SLOT(ServerActivateDeactivate()));
    //connect(ui->pushButtonServerSend              , SIGNAL(clicked(bool))  , this, SLOT(ServerSend()));
    connect(server                                , SIGNAL(newConnection()), this, SLOT(ServerNewConnection()));

    //connect(ui->SliderX, SIGNAL(sliderReleased()), this, SLOT(ServerNewValue()));
    connect(ui->SliderX, SIGNAL(valueChanged(int)), this, SLOT(SliderXSend()));
    connect(ui->SliderY, SIGNAL(valueChanged(int)), this, SLOT(SliderYSend()));

    connect(ui->pushButtonArmed, SIGNAL(clicked(bool)), this, SLOT(ArmedSendClicked()));
    connect(ui->pushButtonShoot, SIGNAL(pressed()), this, SLOT(ShootSendOne()));
    connect(ui->pushButtonShoot, SIGNAL(released()),this,SLOT(ShootSendZero()));

    //connect(ui->pushButtonClientConnectDisconnect, SIGNAL(clicked(bool)), this, SLOT(ClientConnectDisconnect()));
    //connect(ui->pushButtonClientSend             , SIGNAL(clicked(bool)), this, SLOT(ClientSend()));
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

//////// no se empleará
/*
void MainWindow::ServerSend(void)
{
    //ui->plainTextEditServer->insertPlainText(QString("User: Send\nwrite( ") + ui->lineEditServerMessageToSend->text() + " )\n");
    d -> EditPlainTextServer(QString("User: Send\nwrite( ") + ui->lineEditServerMessageToSend->text() + " )\n");

    QByteArray data = ui->lineEditServerMessageToSend->text().toUtf8();
    QList<QObject*> connections = server->children();
    foreach (QObject *connection, connections)
        ((CConnection*)connection)->write(data);
}
*/

///////// NO SE EMPLEARÁ
/*
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
*/

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

/*
bool MainWindow::ClientSend(void)
{
    QByteArray data = ui->lineEditClientMessageToSend->text().toUtf8();
    return client->write(data) == data.size();
}
*/

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

    //ui->plainTextEditClient->insertPlainText(QString("Read: ") + read + '\n');
    d -> EditPlainTextClient(QString("Read: ") + read + '\n');
}


void MainWindow::on_pushButtonSetting_clicked()
{
    d->open();
}

/*
void MainWindow::setSliderControladoX(int value)
{
    ui -> SliderControladoX -> setValue(value);
}

void MainWindow::setSliderControladoY(int value)
{
    ui -> SliderControladoY -> setValue(value);
}
*/

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

void MainWindow::closeEvent (QCloseEvent *event)
{
    event -> accept(); // Se acepta el evento
    d->close(); // Se cierra la ventana "Configuracion IP"
}
