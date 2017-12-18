#ifndef SERIAL_H
#define SERIAL_H

#include <QMainWindow>
#include <QByteArray>
#include <QString>
#include <QTimer>

class QSerialPort;

namespace Ui {
class Serial;
}

class Serial : public QMainWindow
{
    Q_OBJECT

public:
    explicit Serial(QWidget *parent = 0);
    ~Serial();

private slots:
    void clock();

    void portclose();

    void on_bt_connect_clicked();

    void on_bt_disconect_clicked();

    void update();

    void on_sand_clicked();

    void on_feedFish_clicked();

    void on_start_clicked();

    void on_reboot_clicked();

    void on_setSalt_clicked();

    void on_setTemp_clicked();

private:
    Ui::Serial *ui;
    QSerialPort *serialport;
    QByteArray requestData;
    QString msg;
    QTimer *timer;
};

#endif // SERIAL_H
