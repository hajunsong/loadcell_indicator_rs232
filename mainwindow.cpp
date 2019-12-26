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

	ui->plot->addGraph();
	ui->plot->graph(0)->setScatterStyle(QCPScatterStyle::ssDot);
	ui->plot->graph(0)->setLineStyle(QCPGraph::lsLine);

	QPen blueDotPen;
	blueDotPen.setColor(QColor(0, 0, 255, 255));
	blueDotPen.setStyle(Qt::SolidLine);
	blueDotPen.setWidthF(4);
	ui->plot->graph(0)->setPen(blueDotPen);
	ui->plot->graph(0)->setName("Collision force");
	ui->plot->xAxis->setRange(-1, 1);
	ui->plot->yAxis->setRange(-1, 1);
	ui->plot->replot();
	ui->plot->xAxis->setLabel("Time [s]");
	ui->plot->yAxis->setLabel("Force [N]");

	time = 0;

	connect(ui->btnStart, SIGNAL(clicked()), this, SLOT(btnStartClicked()));
	connect(ui->btnSave, SIGNAL(clicked()), this, SLOT(btnSaveClicked()));
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
    }
    else{
        connectState = comm->connect(ui->comboPort->currentText(), ui->comboBaud->currentText());
		disconnect(comm->serial, SIGNAL(readyRead()), this, SLOT(readyRead()));
		ui->btnStart->setText("Start");

    }
    ui->btnConnect->setText(connectState ? "Disconnect" : "Connect");
}

void MainWindow::readyRead(){
	ui->rbComState->toggle();

	QByteArray rxData = comm->serial->readAll();
	if (rxData.length() == 10){
		qDebug() << rxData.split('\r')[0].toDouble();
		x.push_back(time);
        y.push_back(rxData.split('\r')[0].toDouble()*9.80665);
		time += 0.01;

		ui->plot->graph(0)->setData(x, y);
		vector<double> x_temp, y_temp;
		x_temp = x.toStdVector();
		y_temp = y.toStdVector();
		sort(y_temp.begin(), y_temp.end());

		ui->plot->xAxis->setRange(x_temp.front(), x_temp.back());
		ui->plot->yAxis->setRange(y_temp.front(), y_temp.back());

		ui->plot->replot();
	}
}

void MainWindow::btnStartClicked(){
	if(ui->btnStart->text().compare("Start") == 0){
		connect(comm->serial, SIGNAL(readyRead()), this, SLOT(readyRead()));
		ui->btnStart->setText("Stop");
	}
	else if(ui->btnStart->text().compare("Stop") == 0){
		disconnect(comm->serial, SIGNAL(readyRead()), this, SLOT(readyRead()));
		ui->btnStart->setText("Start");
	}
}

void MainWindow::btnSaveClicked(){
	ui->btnSave->setEnabled(false);
	FILE *fp;
	fp = fopen("save_data.csv","w+");
	for(int i = 0; i < x.length(); i++){
        fprintf(fp, "%.7f,%.7f\n", x[i], y[i]*9.80665);
	}
	fclose(fp);
	ui->btnSave->setEnabled(true);
}
