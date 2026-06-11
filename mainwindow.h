#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include "mycustomplot.h"
#include "Define.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 将十六进制 ASCII 字符串转换为原始字节数组
    QByteArray hexAsciiToBytes(const QByteArray &hexAscii);
    // 将原始字节数组解析为蓝牙数据帧
    BleDataFrame parseBleFrame(const QByteArray &data);
    void plotDotLineChart(BleDataFrame frame);
    // 创建新Excel文件并写入表头
    void createNewExcelFile(const QString &filePath);
    // 校验Excel文件的列头
    bool validateExcelHeaders(const QString &filePath);
    bool isLabelExist(const QString &xlsxDoc, const QString &targetLabel);
    // 导出QCustomPlot图像到临时PNG文件
    bool exportPlotToImage(QCustomPlot *customPlot, int width, int height);

private slots:
    void addDevice(const QBluetoothDeviceInfo& info);
    void scanFinished();
    void deviceConnected();
    void deviceDisconnected();
    void serviceDiscovered(const QBluetoothUuid &uuid);
    void serviceScanDone();
    void characteristicRead(const QLowEnergyCharacteristic &info, const QByteArray &value);
    void characteristicWritten(const QLowEnergyCharacteristic &info, const QByteArray &value);
    void characteristicChanged(const QLowEnergyCharacteristic &info, const QByteArray &value);
    void descriptorWritten(const QLowEnergyDescriptor &descriptor, const QByteArray &value);
    void onErrorOccurred(QLowEnergyController::Error error);
    void onConnectionStateChanged(QLowEnergyController::ControllerState state);

    void on_serviceTree_itemClicked(QTreeWidgetItem *item, int column);

    void on_scanButton_clicked();

    void on_connectButton_clicked();

    // void on_readButton_clicked();

    void on_writeButton_clicked();

    void on_subscribeButton_clicked();

    void on_clearButton_clicked();

    void on_setPathButton_clicked();

    void on_saveButton_clicked();

private:
    Ui::MainWindow *ui;

    void updateConnectionUI(bool connected);
    QByteArray hexStringToByteArray(const QString &hex);

    //Blutooth components
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QLowEnergyController* controller;
    QLowEnergyService* currentService;
    // Current device & service info
    QString selectedDeviceAddress;
    QBluetoothDeviceInfo selectedDeviceInfo;
    QLowEnergyCharacteristic selectedCharacteristic;
    bool isSubscribed;
    //存储文件路径（包含文件名）
    QString m_fileFolderPath;
};

#endif // MAINWINDOW_H
