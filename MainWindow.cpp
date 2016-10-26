#include <QtSerialPort>
#include <QtWidgets>
#include <fstream>
#include <sstream>
#include <qhexedit.h>
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "intelhexclass.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	serialPort(nullptr),
	EEPROMPages(16)
{
	ui->setupUi(this);
	querySerialPorts();
	hexEdit = new QHexEdit(this);
	hexEdit->setMinimumWidth(800);
	ui->centralWidget->layout()->addWidget(hexEdit);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_serialConnect_clicked()
{
    if(!serialPort)
    {
        serialPort = new QSerialPort(ui->serialCombo->currentText(), nullptr);
        serialPort->setBaudRate(115200);
        serialPort->open(QIODevice::ReadWrite);
        querySerialPorts();

        ui->serialConnect->setText("Disconnect");
    }
    else
    {
		//serialPort->flush();
        serialPort->close();
        delete serialPort;
        serialPort = nullptr;
        querySerialPorts();

        ui->serialConnect->setText("Connect");
    }
}

void MainWindow::on_idButton_clicked()
{
    if(serialPort)
    {
        serialPort->write("oeic");
        serialPort->waitForReadyRead(1000);
        auto arr = serialPort->readAll();
        while(serialPort->waitForReadyRead(100))
            arr.append(serialPort->readAll());
        if(arr.length() != 3)
            qDebug() << "error";
        else
        {
            ui->statusBar->showMessage(QString(arr.toHex()));
        }
        ui->statusBar->showMessage(QString(arr.toHex()));
    }
}

void MainWindow::on_actionOpen_Program_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Program"), QString(), tr("Intel Hex Files (*.hex *.ihx)"));
    std::ifstream ifProg;
    ifProg.open(fileName.toLocal8Bit().data(), std::ifstream::in);
    if(ifProg.good())
    {
        intelhex inthex;
        ifProg >> inthex;
        unsigned long sa, ea;
        inthex.startAddress(&sa);
        inthex.endAddress(&ea);
        qDebug() << "Address: " << sa << " - " << ea;

        unsigned long len = ea - sa + 1;
        unsigned long r = len % 256;
        len /= 256;
        if(r)
            len += 1;
        len *= 256;

        programBinary.clear();
        programBinary.fill(0, len);
        for(unsigned int i = 0; i <= ea; i++)
        {
            unsigned char d;
            if(inthex.getData(&d, i))
                programBinary[i] = d;
            else
                programBinary[i] = 0;
        }
        hexEdit->setData(programBinary);
    }
}

void MainWindow::on_action4K_triggered()
{
	EEPROMPages = 16;
	ui->action4K->setChecked(true);
	ui->action8K->setChecked(false);
}

void MainWindow::on_action8K_triggered()
{
	EEPROMPages = 32;
	ui->action4K->setChecked(false);
	ui->action8K->setChecked(true);
}


void MainWindow::on_readButton_clicked()
{
    if(serialPort)
    {
        QByteArray data;
        serialPort->write("oe");
        for(uint8_t i = 0; i < EEPROMPages; i++)
        {
            QByteArray combo;
            combo.push_back('x');
            combo.push_back(i);
            combo.push_back('b');
            combo.push_back('r');
            serialPort->write(combo);
            serialPort->waitForBytesWritten(1000);
			while (serialPort->waitForReadyRead(100))
				data.append(serialPort->readAll());
			ui->statusBar->showMessage(QString("Read page: ") + QString::number(i));
        }
        serialPort->write("c");
        hexEdit->setData(data);
    }
}

void MainWindow::on_downloadButton_clicked()
{
    if(serialPort)
    {
        QByteArray data;
        serialPort->write("oe");
        int pages = programBinary.length() / 256;
        qDebug() << programBinary.length() << pages;
        for(uint8_t i = 0; i < pages; i++)
        {
            QByteArray combo;
            combo.push_back('x');
            combo.push_back(i);
            combo.push_back('w');
            for(int p = 0; p < 256; p++)
                combo.push_back(programBinary[i * 256 + p]);
            combo.push_back('a');

            serialPort->write(combo);
            serialPort->waitForBytesWritten(1000);
            while(serialPort->waitForReadyRead(100))
                data.append(serialPort->readAll());
			ui->statusBar->showMessage(QString(data));
        }
        serialPort->write("c");
    }
}

void MainWindow::on_eraseButton_clicked()
{
    if(serialPort)
    {
        serialPort->write("oepc");
        serialPort->waitForReadyRead(1000);
        QByteArray d = serialPort->read(1);
        if(d[0] == '.')
            ui->statusBar->showMessage("OK");
        else
            qDebug() << "Error";
    }
}

void MainWindow::querySerialPorts()
{
    for(auto* p : serialLabels)
        delete p;
	serialLabels.clear();
    ui->serialCombo->clear();

    auto* layout = ui->serialLayout;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        QString s = QObject::tr("Port: ") + info.portName() + "\n"
                    + QObject::tr("Location: ") + info.systemLocation() + "\n"
                    + QObject::tr("Description: ") + info.description() + "\n"
                    + QObject::tr("Manufacturer: ") + info.manufacturer() + "\n"
                    + QObject::tr("Serial number: ") + info.serialNumber() + "\n"
                    + QObject::tr("Vendor Identifier: ") + (info.hasVendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString()) + "\n"
                    + QObject::tr("Product Identifier: ") + (info.hasProductIdentifier() ? QString::number(info.productIdentifier(), 16) : QString()) + "\n"
                    + QObject::tr("Busy: ") + (info.isBusy() ? QObject::tr("Yes") : QObject::tr("No")) + "\n";

        QLabel *label = new QLabel(s);
        serialLabels.push_back(label);
        layout->addWidget(label);
        ui->serialCombo->addItem(info.portName());
    }
}
