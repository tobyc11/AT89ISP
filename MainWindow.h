#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QtSerialPort>
#include <vector>

class QHexEdit;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_serialConnect_clicked();
    void on_idButton_clicked();
    void on_actionOpen_Program_triggered();
    void on_readButton_clicked();
    void on_downloadButton_clicked();
    void on_eraseButton_clicked();

private:
    std::vector<QLabel*> serialLabels;
    void querySerialPorts();

    Ui::MainWindow *ui;
    QSerialPort* serialPort;
    QByteArray programBinary;
    QHexEdit* hexEdit;
};

#endif // MAINWINDOW_H
