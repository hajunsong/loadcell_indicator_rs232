#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	QList<QString> portItems = {"/dev/ttyUSB0"};
    QList<QString> baudItems = {"115200"};
    ui->comboPort->addItems(portItems);
    ui->comboBaud->addItems(baudItems);

    comm = new SerialComm();
    connectState = false;

    connect(ui->btnConnect, SIGNAL(clicked()), this, SLOT(btnConnectClicked()));

    updateTimer = new QTimer(this);
    updateTimer->setInterval(1000);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateTimeout()));

    connect(comm->serial, SIGNAL(readyRead()), this, SLOT(readyRead()));

	ui->plot->addGraph();
	ui->plot->graph(0)->setScatterStyle(QCPScatterStyle::ssDot);
	ui->plot->graph(0)->setLineStyle(QCPGraph::lsLine);

	QPen blueDotPen;
	blueDotPen.setColor(QColor(30, 40, 255, 150));
	blueDotPen.setStyle(Qt::DotLine);
	blueDotPen.setWidthF(4);
	ui->plot->graph(0)->setPen(blueDotPen);
	ui->plot->graph(0)->setName("Collision force");
	ui->plot->xAxis->setRange(-1, 1);
	ui->plot->yAxis->setRange(-1, 1);
	ui->plot->replot();
	ui->plot->xAxis->setLabel("Time [s]");
	ui->plot->yAxis->setLabel("Force [N]");
}

MainWindow::~MainWindow()
{
    delete comm;

    delete ui;
}

void MainWindow::btnConnectClicked()
{
    if (connectState){
        comm->disconnect();
        connectState = false;
//        updateTimer->stop();
    }
    else{
        connectState = comm->connect(ui->comboPort->currentText(), ui->comboBaud->currentText());
//        updateTimer->start();

    }
    ui->btnConnect->setText(connectState ? "Disconnect" : "Connect");
}

void MainWindow::updateTimeout()
{
    ui->rbComState->toggle();

    comm->writeread();
}

void MainWindow::readyRead(){
	ui->rbComState->toggle();

//    QString received;
//    while(comm->serial->canReadLine()){
//        qDebug() << comm->serial->canReadLine();
//        received = comm->serial->readLine();
//    }
//    qDebug() << received;
	QByteArray rxData = comm->serial->readAll();
	qDebug() << rxData << ", " << rxData.length();
}
