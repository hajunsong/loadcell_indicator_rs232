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
//    qDebug() << rxData;
//    unsigned char uart_rx_buffer[100]; // receive buffer for uart communication
    vector<unsigned char> uart_rx_buffer;
    for(int i = 0; i < rxData.length(); i++){
        uart_rx_buffer.push_back(static_cast<unsigned char>(rxData.at(i)));
    }
    if(rxData.at(0) == 0x55){
//        qDebug() << "check SOP";
        if(rxData.at(1) == 0x0b){
//            qDebug() << "check ID";
            unsigned char data_field[16]; // storage buffer for data field
            // check the SOP, EOP, Checksum of received UART data
            // SOP == 0x55, EOP == 0xAA, Checksum == summation of each data in data_field
            // Save the data field's data in data field buffer
            for(unsigned int idx = 0; idx < 16; idx++){
                data_field[idx] = uart_rx_buffer[idx + 1]; //in case that rx_buffer[0] is SOP
            }
            // data field processing
            short raw_data[6] = { 0 };
            unsigned short temp;
            unsigned DF=50, DT=1000; // DF, DT depend on the model, refer to 3.6.11
//            // response ID checking
//            if( (data_field[0] != 10) || (data_field[0] != 11) )
//                return;
            for (int idx = 0; idx < 6; idx++)
            {
                temp = data_field [2 * idx + 1] * 256;
                temp += data_field [2 * idx + 2];
                raw_data[idx] = static_cast<signed short>(temp); // casting process
            }
            // Conversion from signed short data to float data and data scaling
            // Set Force/Torque Original
            double ft_array[6];
            for (int n = 0; n < 3; n++)
            {
                ft_array[n] = ((static_cast<double>(raw_data[n]))/DF);
                ft_array[n + 3] = ((static_cast<double>(raw_data[n + 3]))/DT);
            }

            double Fx = ft_array[0];
            double Fy = ft_array[1];
            double Fz = ft_array[2];
            double Tx = ft_array[3];
            double Ty = ft_array[4];
            double Tz = ft_array[5];

            printf("\t%f\t%f\t%f\t%f\t%f\t %f\n", Fx, Fy, Fz, Tx, Ty, Tz);
        }
    }

}

void MainWindow::btnStartClicked(){
	if(ui->btnStart->text().compare("Start") == 0){
		connect(comm->serial, SIGNAL(readyRead()), this, SLOT(readyRead()));
		ui->btnStart->setText("Stop");

        QByteArray txData;
        txData.append(QByteArray::fromRawData("\x55", 1));
        txData.append(QByteArray::fromRawData("\x0b", 1));
        txData.append(QByteArray::fromRawData("\x00", 1));
        txData.append(QByteArray::fromRawData("\x00", 1));
        txData.append(QByteArray::fromRawData("\x00", 1));
        txData.append(QByteArray::fromRawData("\x00", 1));
        txData.append(QByteArray::fromRawData("\x00", 1));
        txData.append(QByteArray::fromRawData("\x00", 1));
        txData.append(QByteArray::fromRawData("\x00", 1));
        txData.append(QByteArray::fromRawData("\x0b", 1));
        txData.append(QByteArray::fromRawData("\xAA", 1));

//        QByteArray txData = QByteArray::fromRawData("\x55\x0b\x00\x00\x00\x00\x00\x00\x00\x0b\xAA", 11);
        comm->serial->write(txData);
        qDebug() << txData.toHex();
	}
	else if(ui->btnStart->text().compare("Stop") == 0){
		disconnect(comm->serial, SIGNAL(readyRead()), this, SLOT(readyRead()));
		ui->btnStart->setText("Start");

        QByteArray txData;
        txData.append(QByteArray::fromRawData("\x55", 1));
        txData.append(QByteArray::fromRawData("\x0c", 1));
        txData.append(QByteArray::fromRawData("\x00", 1));
        txData.append(QByteArray::fromRawData("\x00", 1));
        txData.append(QByteArray::fromRawData("\x00", 1));
        txData.append(QByteArray::fromRawData("\x00", 1));
        txData.append(QByteArray::fromRawData("\x00", 1));
        txData.append(QByteArray::fromRawData("\x00", 1));
        txData.append(QByteArray::fromRawData("\x00", 1));
        txData.append(QByteArray::fromRawData("\x0c", 1));
        txData.append(QByteArray::fromRawData("\xAA", 1));
        comm->serial->write(txData);
        qDebug() << txData.toHex();
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
