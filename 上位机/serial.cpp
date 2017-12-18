#include "serial.h"
#include "ui_serial.h"
#include <QMessageBox>
#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QByteArray>

Serial::Serial(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Serial),
    timer(new QTimer)
{
    serialport = new QSerialPort();
    ui->setupUi(this);
    ui->bt_disconect->setDisabled(true);
    ui->sand->setDisabled(true);
    ui->start->setDisabled(true);
    connect(timer,&QTimer::timeout,this,&Serial::clock);
    connect(serialport,SIGNAL(readyRead()),this,SLOT(update()));
    connect(serialport,&QIODevice::aboutToClose,this,&Serial::portclose);
}

Serial::~Serial()
{
    delete ui;
}

void Serial::clock()
{
    //serialport->write("state now\r\n");
}

void Serial::portclose()
{
    timer->stop();
}

void Serial::on_bt_connect_clicked()
{
    QString port = ui->serial->text();
    serialport->setPortName(port);
    if(serialport->open(QIODevice::ReadWrite)){
        serialport->setBaudRate(QSerialPort::Baud115200);
        serialport->setDataBits(QSerialPort::Data8);
        serialport->setStopBits(QSerialPort::OneStop);
        serialport->setParity(QSerialPort::NoParity);
        serialport->setFlowControl(QSerialPort::NoFlowControl);

        ui->bt_connect->setDisabled(true);
        ui->bt_disconect->setDisabled(false);
        ui->sand->setDisabled(false);
        ui->start->setDisabled(false);
        timer->stop();
        timer->start(1000);

    }else
        QMessageBox::warning(nullptr,"error","Can not open SerialPort",QMessageBox::Retry);
}

void Serial::update()
{
    QStringList string;
    requestData = serialport->readAll();  //读取数据
    QString buf(requestData);
    if(!buf.endsWith('\n')){
        msg+=buf;
    }
    else{
        msg+=buf;
        string = msg.split("\r\n");
        msg.clear();

        for(QString buf:string)
        {
            if(buf!= NULL)
            {
                if(buf.startsWith("pro")){
                    QStringList sett = buf.split(" ");
                    try{
                        ui->area->setText(sett[1]);
                        ui->temp->setText(sett[2]);
                        ui->salt->setText(sett[3]);
                        if(ui->start->text() == "运行"){
                            ui->start->setText("停止");
                        }
                        if(ui->tempMin->text() ==""){
                            serialport->write("get\r\n");
                        }
                    }catch(...){

                    }

                }else if(buf.startsWith("param")){
                    qDebug()<<buf;
                    QStringList sett = buf.split(" ");
                    try{
                        ui->saltMax->setText(sett[1]);
                        ui->saltMin->setText(sett[2]);
                        ui->tempMax->setText(sett[3]);
                        ui->tempMin->setText(sett[4]);

                    }catch(...){

                    }
                }else{
                     ui->message->append(buf);
                }

            }

        }
    }
    requestData.clear();
}


/*
*   断开连接
**/
void Serial::on_bt_disconect_clicked()
{
    serialport->close();

    ui->bt_disconect->setDisabled(true);
    ui->sand->setDisabled(true);
    ui->bt_connect->setDisabled(false);
    ui->start->setDisabled(true);
}

void Serial::on_sand_clicked()
{
    serialport->write((ui->et_send->text()+"\r\n").toLatin1());
}

void Serial::on_feedFish_clicked()
{
    serialport->write("fish\r\n");
}

void Serial::on_start_clicked()
{
    if(ui->start->text() == "运行"){
        serialport->write("run\r\n");
        ui->start->setText("停止");
    }else{

        serialport->write("stop\r\n");
        ui->start->setText("运行");
    }
}

void Serial::on_reboot_clicked()
{
    serialport->write("reboot\r\n");
    ui->start->setText("运行");
}

void Serial::on_setSalt_clicked()
{

    serialport->write(("setSalt "+ui->saltMin->text()+" "+ui->saltMax->text()+"\r\n").toLatin1());
}

void Serial::on_setTemp_clicked()
{
    serialport->write(("setTemp "+ui->tempMin->text()+" "+ui->tempMax->text()+"\r\n").toLatin1());
}
