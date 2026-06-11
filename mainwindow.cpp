#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "msgbox.h"
#include <QFileDialog>
#include <QSettings>
#include <QFileInfo>
#include <QCoreApplication>
#include "3rdparty/xlsx/xlsxdocument.h"
#include "wordengine.h"

class ExcelHelper {
public:
    // 定义需要的列头
    static const QStringList RequiredHeaders;
};

const QStringList ExcelHelper::RequiredHeaders = {"业务标签", "检测结果", "压力", "直径", "结果"};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->serviceTree->setHeaderLabels(QStringList()<<"服务"<<"值");
    ui->serviceTree->setColumnWidth(0, 400);
    ui->serviceTree->setIndentation(20);
    ui->serviceTree->setAlternatingRowColors(true);

    discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    discoveryAgent->setLowEnergyDiscoveryTimeout(5000);
    controller = nullptr;
    currentService = nullptr;
    isSubscribed = false;
    ui->groupBox->setEnabled(false);
    ui->setPathButton->setVisible(false);
    ui->saveButton->setEnabled(false);

    connect(discoveryAgent,&QBluetoothDeviceDiscoveryAgent::deviceDiscovered,this,&MainWindow::addDevice);
    connect(discoveryAgent,&QBluetoothDeviceDiscoveryAgent::finished,this,&MainWindow::scanFinished);
    connect(discoveryAgent,QOverload<QBluetoothDeviceDiscoveryAgent::Error>::of(&QBluetoothDeviceDiscoveryAgent::errorOccurred),
            this,[this](QBluetoothDeviceDiscoveryAgent::Error error){
        qDebug()<<QString("扫描错误:%1").arg(discoveryAgent->errorString());
    });

    ui->plotWidget->xAxis->setLabel("时间/s");
    ui->plotWidget->yAxis->setLabel("气泡数/个");
    ui->plotWidget->xAxis->setRange(0,60);
    ui->plotWidget->yAxis->setRange(0,90);

    m_fileFolderPath = "";
}

MainWindow::~MainWindow()
{
    if(controller)
    {
        delete controller;
        controller = nullptr;
    }
    delete ui;
}

QByteArray MainWindow::hexAsciiToBytes(const QByteArray &hexAscii)
{
    QByteArray cleaned = hexAscii;
    // 长度必须是偶数
    if (cleaned.size() % 2 != 0) {
        qDebug() << "hexAsciiToBytes: 长度不是偶数";
        return QByteArray();
    }
    QByteArray result;
    result.reserve(cleaned.size() / 2);
    for (int i = 0; i < cleaned.size(); i += 2) {
        bool ok;
        char byte = static_cast<char>(cleaned.mid(i, 2).toUInt(&ok, 16));
        if (!ok) {
            qDebug() << "hexAsciiToBytes: 无效的十六进制字符";
            return QByteArray();
        }
        result.append(byte);
    }
    return result;
}

BleDataFrame MainWindow::parseBleFrame(const QByteArray &data)
{
    BleDataFrame frame;
    frame.valid = false;

    // 协议：AA + 3字节idx + 4字节timestamp + 1字节press_type + 1字节pip_type + 60字节bubble + 1字节test_result + 1字节sum + BB
    const int expectedLen = 1 + 3 + 4 + 1 + 1 + 60 + 1 + 1 + 1; // 73字节
    if (data.size() != expectedLen) {
        qDebug() << "数据长度错误，期望" << expectedLen << "实际" << data.size();
        return frame;
    }

    // 检查帧头帧尾
    if ((quint8)data[0] != 0xAA || (quint8)data[expectedLen - 1] != 0xBB) {
        qDebug() << "帧头或帧尾错误";
        return frame;
    }

    // 提取字段
    int pos = 1;
    frame.idx = data.mid(pos, 3); pos += 3;

    // 时间戳：4字节大端（文档示例：1752063972 存储为 e4 5f 6e 68，注意是小端还是大端？示例给出的字节顺序是 e4 5f 6e 68，即 0xe4,0x5f,0x6e,0x68，对应数值：0x686e5fe4 = 1752063972，说明是**大端**（网络字节序））
    frame.timestamp = (quint32)((quint8)data[pos]) |
                      ((quint8)data[pos+1] << 8) |
                      ((quint8)data[pos+2] << 16) |
                      (quint8)data[pos+3] << 24;
    pos += 4;

    frame.pressType = (quint8)data[pos]; pos++;
    frame.pipType = (quint8)data[pos]; pos++;
    frame.bubbleData = data.mid(pos, 60); pos += 60;
    frame.testResult = (quint8)data[pos]; pos++;
    frame.sum = (quint8)data[pos]; pos++;

    // // 校验和计算（假设校验范围从 idx 到 testResult，即从第1个字节到第69个字节，共3+4+1+1+60+1 = 70字节）
    // quint8 calcSum = 0;
    // for (int i = 1; i < expectedLen - 2; ++i) { // 从 idx 开始到 testResult（不包括头AA和尾BB和sum本身）
    //     calcSum += (quint8)data[i];
    // }
    // calcSum %= 255; // 余255
    // if (calcSum != frame.sum) {
    //     qDebug() << "校验和错误：计算值" << calcSum << "接收值" << frame.sum;
    //     return frame;
    // }

    frame.valid = true;
    return frame;
}

void MainWindow::plotDotLineChart(BleDataFrame frame)
{
    // 提取气泡数据并绘制
    QVector<double> x(60),y(60);
    for(int i=0;i<60;++i)
    {
        x[i]=i;
        y[i]=static_cast<quint8>(frame.bubbleData[i]);
    }
    ui->plotWidget->clearGraphs();
    QCPGraph *graph = ui->plotWidget->addGraph();
    graph->setData(x,y);
    graph->setPen(QPen(Qt::blue, 2));
    graph->setScatterStyle(QCPScatterStyle::ssCircle);
    if((int)y[59]>90)
    {
        ui->plotWidget->yAxis->setRange(0,y[59]+5);
    }
    QDateTime dt = QDateTime::fromSecsSinceEpoch(frame.timestamp);
    dt = dt.addSecs(-8*3600);
    QString missionIdText = frame.idx.toHex().toUpper()+dt.toString("yyyyMMddHHmmss");
    QString detectResult = QString::number(y[59])+"气泡数/min";
    QString pressureText = "<=6.9MPa";
    switch (frame.pressType) {
    case 0:
        pressureText="<=6.9MPa";
        break;
    case 1:
        pressureText=">6.9~10.3MPa";
        break;
    case 2:
        pressureText=">10.3~13.8MPa";
        break;
    case 3:
        pressureText=">13.8~17.2MPa";
        break;
    case 4:
        pressureText=">17.2~20.7MPa";
        break;
    case 5:
        pressureText=">20.7~27.6MPa";
        break;
    case 6:
        pressureText=">27.6~34.5MPa";
        break;
    case 7:
        pressureText=">34.5~41.4MPa";
        break;
    default:
        break;
    }
    QString pipText = frame.pipType==0?"<=18mm":">18mm";
    QString result = frame.testResult==1?"合格":"不合格";
    ui->missionIDLabel->setText(missionIdText);
    ui->detectResultLabel->setText(detectResult);
    ui->pressureLabel->setText(pressureText);
    ui->diameterLabel->setText(pipText);
    ui->resultLabel->setText(result);
    ui->plotWidget->replot();
}

void MainWindow::createNewExcelFile(const QString &filePath)
{
    QXlsx::Document xlsx;
    // 写入列头
    for (int i = 0; i < ExcelHelper::RequiredHeaders.size(); ++i) {
        xlsx.write(1, i + 1, ExcelHelper::RequiredHeaders[i]);
    }
    // 保存文件
    if (!xlsx.saveAs(filePath)) {
        qDebug() << "创建Excel文件失败:" << filePath;
    }
}

bool MainWindow::validateExcelHeaders(const QString &filePath)
{
    QXlsx::Document xlsx(filePath);
    if (!xlsx.dimension().isValid()) {
        qDebug() << "无法打开Excel文件:" << filePath;
        return false;
    }
    // 读取第一行的列头
    QStringList existingHeaders;
    int maxCol = 20; // 最多检查20列，或直到遇到空单元格
    for (int col = 1; col <= maxCol; ++col) {
        QXlsx::Cell *cell = xlsx.cellAt(1, col);
        if (!cell || cell->value().toString().isEmpty()) {
            break;
        }
        existingHeaders << cell->value().toString();
    }
    // 检查必须的列头是否都存在
    for (const QString &required : ExcelHelper::RequiredHeaders) {
        if (!existingHeaders.contains(required)) {
            qDebug() << "缺失必须的列头:" << required;
            return false;
        }
    }
    // 所有必须列头都存在，校验通过（允许有多余的列）
    return true;
}

bool MainWindow::isLabelExist(const QString &xlsxDoc, const QString &targetLabel)
{
    QXlsx::Document xlsx(xlsxDoc);
    if (!xlsx.dimension().isValid()) {
        qDebug() << "无法打开Excel文件:" << xlsxDoc;
        return false;
    }
    int targetColumn = -1;
    int maxColumn = 20; // 最多检查20列，可以按需调整
    for (int col = 1; col <= maxColumn; ++col) {
        QXlsx::Cell *cell = xlsx.cellAt(1, col);
        if (cell && cell->value().toString() == "业务标签") {
            targetColumn = col;
            break;
        }
    }

    // 如果找不到“业务标签”列，校验失败，直接返回false
    if (targetColumn == -1) {
        qWarning() << "Excel文件中未找到'业务标签'列头";
        return false;
    }

    // 2. 遍历目标列，查找重复项
    int currentRow = 2; // 数据从第二行开始
    while (true) {
        QXlsx::Cell *cell = xlsx.cellAt(currentRow, targetColumn);
        if (!cell) {
            break; // 单元格为空，表示此列内容结束，跳出循环
        }

        QString labelValue = cell->value().toString();
        if (labelValue == targetLabel) {
            return true; // 找到重复项
        }
        currentRow++;
    }

    return false; // 未发现重复
}

bool MainWindow::exportPlotToImage(QCustomPlot *customPlot, int width, int height)
{
    // 创建临时文件（保存在系统临时目录）
    QTemporaryFile tempFile;
    tempFile.setFileTemplate(QCoreApplication::applicationDirPath()+"/"+ui->missionIDLabel->text()+"1.png");
    if (tempFile.open()) {
        tempFile.close();
        QString filePath = tempFile.fileName();
        // 导出图像，指定宽高
        customPlot->savePng(filePath,width,height);
        qDebug() << "图像已导出:" << filePath;
        // 注意：QTemporaryFile 在销毁时会自动删除文件，因此需要保留文件名或将其转换为常规文件
        // 这里为了后续使用，直接返回文件路径，但在函数结束时 tempFile 对象会被销毁，文件会被删除。
        // 解决方案：使用普通 QFile 并手动管理，或在此处不关闭 temporary 对象。
        // 修正实现：使用普通文件
        QString permanentPath = QCoreApplication::applicationDirPath()+"/"+ui->missionIDLabel->text()+".png";
        if (QFile::copy(filePath, permanentPath)) {
            qDebug() << "图像已保存到:" << permanentPath;
            return true;
        } else {
            qDebug() << "复制图像文件失败";
            return false;
        }
    }
    qDebug() << "图像导出失败";
    return false;
}

void MainWindow::addDevice(const QBluetoothDeviceInfo &info)
{
    if (info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        if("Bubble_tag" == info.name())
        {
            QString label = QString("%1 - %2").arg(info.name().isEmpty() ? "未知" : info.name(), info.address().toString());
            QListWidgetItem* item = new QListWidgetItem(label,ui->deviceList);
            // 关键: 将完整的 QBluetoothDeviceInfo 存储在 item 中
            item->setData(Qt::UserRole,QVariant::fromValue(info));
        }
    }
}

void MainWindow::scanFinished()
{
    ui->scanButton->setEnabled(true);
    qDebug()<<"扫描结束";
    ui->connectButton->setEnabled(ui->deviceList->count()>0);
}

void MainWindow::deviceConnected()
{
    qDebug()<<"已连接，正在发现服务...";

    if(controller)
    {
        controller->discoverServices();
    }
}

void MainWindow::deviceDisconnected()
{
    qDebug()<<"设备已断开";
    updateConnectionUI(false);
    ui->groupBox->setEnabled(false);
    ui->serviceTree->clear();
    ui->connectButton->setEnabled(true);
    if (controller) {
        controller->disconnect();
        controller->deleteLater();
        controller = nullptr;
    }
    currentService = nullptr;
    isSubscribed = false;
    ui->subscribeButton->setText("连接服务");
}

void MainWindow::serviceDiscovered(const QBluetoothUuid &uuid)
{
    qDebug()<<QString("发现服务: %1").arg(uuid.toString());
}

void MainWindow::serviceScanDone()
{
    qDebug()<<"服务发现完成";
    ui->serviceTree->clear();
    QList<QBluetoothUuid> services = controller->services();
    for (const QBluetoothUuid &uuid : services) {
        QLowEnergyService *service = controller->createServiceObject(uuid);
        if (!service)
            continue;
        if("{0000abf0-0000-1000-8000-00805f9b34fb}" != uuid.toString())
            continue;
        QTreeWidgetItem *serviceItem = new QTreeWidgetItem(ui->serviceTree);
        serviceItem->setText(0, uuid.toString());
        serviceItem->setData(0, Qt::UserRole, QVariant::fromValue(service));
        serviceItem->setExpanded(true);
        // 延迟发现特征值: 连接信号
        connect(service, &QLowEnergyService::characteristicRead, this, &MainWindow::characteristicRead);
        connect(service, &QLowEnergyService::characteristicWritten, this, &MainWindow::characteristicWritten);
        connect(service, &QLowEnergyService::characteristicChanged, this, &MainWindow::characteristicChanged);
        connect(service, &QLowEnergyService::descriptorWritten, this, &MainWindow::descriptorWritten);
        connect(service, &QLowEnergyService::stateChanged, this, [this, service](){
            if (service->state() == QLowEnergyService::RemoteServiceDiscovered) {
                qDebug()<<QString("服务 %1 的特征值已发现").arg(service->serviceUuid().toString());
                QTreeWidgetItem *item = nullptr;
                for (int i = 0; i < ui->serviceTree->topLevelItemCount(); ++i) {
                    if (ui->serviceTree->topLevelItem(i)->data(0, Qt::UserRole).value<QLowEnergyService*>() == service) {
                        item = ui->serviceTree->topLevelItem(i);
                        break;
                    }
                }
                if (item) {
                    const QList<QLowEnergyCharacteristic> chars = service->characteristics();
                    for (const QLowEnergyCharacteristic &ch : chars) {
                        QTreeWidgetItem *charItem = new QTreeWidgetItem(item);
                        QString props;
                        if (ch.properties() & QLowEnergyCharacteristic::Read) props += "Read ";
                        if (ch.properties() & QLowEnergyCharacteristic::Write) props += "Write ";
                        if (ch.properties() & QLowEnergyCharacteristic::WriteNoResponse) props += "WriteCmd ";
                        if (ch.properties() & QLowEnergyCharacteristic::Notify) props += "Notify ";
                        if (ch.properties() & QLowEnergyCharacteristic::Indicate) props += "Indicate ";
                        charItem->setText(0, QString("%1 [%2]").arg(ch.uuid().toString(), props.trimmed()));
                        charItem->setData(0, Qt::UserRole, QVariant::fromValue(ch));
                        // 显示初始值
                        charItem->setText(1, "(未读取)");
                    }
                    item->setExpanded(true);
                }
            }
        });
        service->discoverDetails();
    }
    ui->groupBox->setEnabled(true);
    updateConnectionUI(true);
    ui->connectButton->setEnabled(true);
}

void MainWindow::characteristicRead(const QLowEnergyCharacteristic &info, const QByteArray &value)
{
    qDebug()<<QString("读取特征值 %1 : %2").arg(info.uuid().toString(),QString::fromLatin1(value.toHex(' ')));
    // 更新树显示
    for (int i = 0; i < ui->serviceTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *svcItem = ui->serviceTree->topLevelItem(i);
        for (int j = 0; j < svcItem->childCount(); ++j) {
            QTreeWidgetItem *chItem = svcItem->child(j);
            if (chItem->data(0, Qt::UserRole).value<QLowEnergyCharacteristic>().uuid() == info.uuid()) {
                chItem->setText(1, value.toHex(' '));
                break;
            }
        }
    }
}

void MainWindow::characteristicWritten(const QLowEnergyCharacteristic &info, const QByteArray &value)
{
    qDebug()<<QString("写入特征值 %1 成功: %2").arg(info.uuid().toString(), QString::fromLatin1(value.toHex(' ')));
}

void MainWindow::characteristicChanged(const QLowEnergyCharacteristic &info, const QByteArray &value)
{
    qDebug()<<QString("通知/指示接收 %1 : %2").arg(info.uuid().toString(), QString::fromLatin1(value));
    // 更新树显示
    for (int i = 0; i < ui->serviceTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *svcItem = ui->serviceTree->topLevelItem(i);
        for (int j = 0; j < svcItem->childCount(); ++j) {
            QTreeWidgetItem *chItem = svcItem->child(j);
            if (chItem->data(0, Qt::UserRole).value<QLowEnergyCharacteristic>().uuid() == info.uuid()) {
                chItem->setText(1, value.toHex(' '));
                break;
            }
        }
    }
    if("{0000abf1-0000-1000-8000-00805f9b34fb}" == info.uuid().toString())
    {
        // 第一步：将收到的 ASCII 十六进制字符串解码为原始二进制数据
        QByteArray rawData = hexAsciiToBytes(value);
        if (rawData.isEmpty()) {
            qDebug() << "解码失败，原始数据:" << value;
            return;
        }
        // 第二步：解析二进制数据（73 字节）
        BleDataFrame frame = parseBleFrame(rawData);
        if (!frame.valid) {
            qDebug() << "数据帧无效";
            return;
        }
        // 第三步：提取气泡数据并绘制
        plotDotLineChart(frame);
        if("" != ui->missionIDLabel->text())
        {
            ui->saveButton->setEnabled(true);
        }
    }
}

void MainWindow::descriptorWritten(const QLowEnergyDescriptor &descriptor, const QByteArray &value)
{
    qDebug()<<QString("描述符写入: %1 -> %2").arg(descriptor.uuid().toString(), QString::fromLatin1(value.toHex(' ')));
}

void MainWindow::onErrorOccurred(QLowEnergyController::Error error)
{
    QString errorMsg;
    switch (error) {
    case QLowEnergyController::ConnectionError:
        errorMsg = "连接错误：设备可能已被其他主机连接或不可达";
        break;
    case QLowEnergyController::RemoteHostClosedError:
        errorMsg = "远程设备主动断开";
        break;
    default:
        errorMsg = controller->errorString();
        break;
    }
    qDebug()<<QString("控制器错误: %1").arg(errorMsg);
    // 清理控制器
    if (controller) {
        controller->disconnect();
        controller->deleteLater();
        controller = nullptr;
    }
    currentService = nullptr;
    selectedCharacteristic = QLowEnergyCharacteristic();
    updateConnectionUI(false);
    ui->connectButton->setEnabled(true);
}

void MainWindow::onConnectionStateChanged(QLowEnergyController::ControllerState state)
{
    QString stateStr;
    switch (state) {
    case QLowEnergyController::ConnectingState: stateStr = "正在连接"; break;
    case QLowEnergyController::ConnectedState: stateStr = "已连接"; break;
    case QLowEnergyController::ClosingState: stateStr = "正在断开"; break;
    case QLowEnergyController::UnconnectedState: stateStr = "已断开"; break;
    default: stateStr = "未知";
    }
    qDebug()<<QString("连接状态: %1").arg(stateStr);
}

void MainWindow::on_serviceTree_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    if (!item || item->parent() == nullptr) {
        // 不是特征值项
        selectedCharacteristic = QLowEnergyCharacteristic();
        ui->groupBox->setEnabled(controller && controller->state() == QLowEnergyController::ConnectedState);
        return;
    }
    // 选中特征值时，确保操作组是启用的
    ui->groupBox->setEnabled(true);
    selectedCharacteristic = item->data(0, Qt::UserRole).value<QLowEnergyCharacteristic>();
    bool canRead = selectedCharacteristic.properties() & QLowEnergyCharacteristic::Read;
    bool canWrite = (selectedCharacteristic.properties() & QLowEnergyCharacteristic::Write) ||
                    (selectedCharacteristic.properties() & QLowEnergyCharacteristic::WriteNoResponse);
    bool canSubscribe = (selectedCharacteristic.properties() & QLowEnergyCharacteristic::Notify) ||
                        (selectedCharacteristic.properties() & QLowEnergyCharacteristic::Indicate);
    // ui->readButton->setEnabled(canRead);
    ui->writeButton->setEnabled(canWrite);
    ui->subscribeButton->setEnabled(canSubscribe);
    if (canSubscribe) {
        ui->subscribeButton->setText(isSubscribed ? "断开服务" : "连接服务");
    } else {
        ui->subscribeButton->setText("不支持连接");
    }
    QString selectedCharacteristicString = selectedCharacteristic.uuid().toString();
    qDebug()<<QString("选中特征值: %1").arg(selectedCharacteristicString);
}

void MainWindow::on_scanButton_clicked()
{
    ui->deviceList->clear();
    ui->serviceTree->clear();
    if(controller)
    {
        controller->disconnect();
        controller->deleteLater();
        controller = nullptr;
        currentService = nullptr;
        selectedCharacteristic = QLowEnergyCharacteristic();
        isSubscribed = false;
    }
    // 创建新的 discoveryAgent
    if (discoveryAgent) {
        discoveryAgent->deleteLater();
    }
    discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    discoveryAgent->setLowEnergyDiscoveryTimeout(5000);
    connect(discoveryAgent,&QBluetoothDeviceDiscoveryAgent::deviceDiscovered,this,&MainWindow::addDevice);
    connect(discoveryAgent,&QBluetoothDeviceDiscoveryAgent::finished,this,&MainWindow::scanFinished);
    connect(discoveryAgent,QOverload<QBluetoothDeviceDiscoveryAgent::Error>::of(&QBluetoothDeviceDiscoveryAgent::errorOccurred),
            this,[this](QBluetoothDeviceDiscoveryAgent::Error error){
                qDebug()<<QString("扫描错误:%1").arg(discoveryAgent->errorString());
            });
    updateConnectionUI(false);
    ui->groupBox->setEnabled(false);
    qDebug()<<"开始扫描蓝牙设备";

    QBluetoothDeviceDiscoveryAgent::DiscoveryMethods methods = discoveryAgent->supportedDiscoveryMethods();
    qDebug() << "Supported discovery methods:" << methods;

    discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    ui->scanButton->setEnabled(false);
}

void MainWindow::on_connectButton_clicked()
{
    if(!ui->deviceList->currentItem())
    {
        msgBox::show("提示","请选择一个设备",1);
        return;
    }
    selectedDeviceInfo = ui->deviceList->currentItem()->data(Qt::UserRole).value<QBluetoothDeviceInfo>();
    if(controller){
        controller->disconnect();
        controller->deleteLater();
        controller = nullptr;
    }
    controller = QLowEnergyController::createCentral(selectedDeviceInfo,this);
    connect(controller,&QLowEnergyController::connected,this,&MainWindow::deviceConnected);
    connect(controller, &QLowEnergyController::disconnected, this, &MainWindow::deviceDisconnected);
    connect(controller, &QLowEnergyController::serviceDiscovered, this, &MainWindow::serviceDiscovered);
    connect(controller, &QLowEnergyController::discoveryFinished, this, &MainWindow::serviceScanDone);
    connect(controller, QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::errorOccurred),
            this, &MainWindow::onErrorOccurred);
    connect(controller, &QLowEnergyController::stateChanged, this, &MainWindow::onConnectionStateChanged);
    qDebug()<<QString("正在连接 %1 ...").arg(selectedDeviceInfo.address().toString());
    controller->connectToDevice();
    ui->connectButton->setEnabled(false);
}

// void MainWindow::on_readButton_clicked()
// {
//     if (!selectedCharacteristic.isValid()) {
//         msgBox::show("错误","请先选中一个可读的特征值",1);
//         return;
//     }
//     if (!currentService) {
//         // 找到特征值所属的服务
//         for (int i = 0; i < ui->serviceTree->topLevelItemCount(); ++i) {
//             QTreeWidgetItem *svcItem = ui->serviceTree->topLevelItem(i);
//             for (int j = 0; j < svcItem->childCount(); ++j) {
//                 QTreeWidgetItem *chItem = svcItem->child(j);
//                 if (chItem->data(0, Qt::UserRole).value<QLowEnergyCharacteristic>().uuid() == selectedCharacteristic.uuid()) {
//                     currentService = svcItem->data(0, Qt::UserRole).value<QLowEnergyService*>();
//                     break;
//                 }
//             }
//         }
//     }
//     if (currentService) {
//         currentService->readCharacteristic(selectedCharacteristic);
//         qDebug()<<QString("发起读取: %1").arg(selectedCharacteristic.uuid().toString());
//     }
// }

void MainWindow::on_writeButton_clicked()
{
    if (!selectedCharacteristic.isValid()) {
        msgBox::show("错误","请先选中一个可读的特征值",1);
        return;
    }
    //------------------------额外逻辑：设置蓝牙外设为当前时间-----------------------------------------
    if ("设置当前时间" == ui->writeButton->text())
    {
        if (!currentService) {
            for (int i = 0; i < ui->serviceTree->topLevelItemCount(); ++i) {
                QTreeWidgetItem *svcItem = ui->serviceTree->topLevelItem(i);
                for (int j = 0; j < svcItem->childCount(); ++j) {
                    QTreeWidgetItem *chItem = svcItem->child(j);
                    if (chItem->data(0, Qt::UserRole).value<QLowEnergyCharacteristic>().uuid() == selectedCharacteristic.uuid()) {
                        currentService = svcItem->data(0, Qt::UserRole).value<QLowEnergyService*>();
                        break;
                    }
                }
            }
        }
        if (currentService) {
            QLowEnergyService::WriteMode mode = QLowEnergyService::WriteWithResponse;
            qint64 timestamp = QDateTime::currentSecsSinceEpoch();
            // 将时间戳转为4字节大端数组
            QByteArray timeBytes;
            timeBytes.resize(4);
            timeBytes[0] = (timestamp >> 24) & 0xFF;
            timeBytes[1] = (timestamp >> 16) & 0xFF;
            timeBytes[2] = (timestamp >> 8) & 0xFF;
            timeBytes[3] = timestamp & 0xFF;
            // 构造响应包 (二进制)
            QByteArray response;
            response.append(char(0xAA));
            response.append(char(0x52));
            response.append(char(0x01));
            response.append(timeBytes);
            response.append(char(0xBB));
            //转换为十六进制字符串（大写，无分隔符）ASCAll
            QByteArray hexString = response.toHex().toUpper();
            currentService->writeCharacteristic(selectedCharacteristic, hexString, mode);
            qDebug()<<QString("设置时间字符串: %1").arg(hexString);
        }
        return;
    }
    //---------------------------------------------------------------------------------------
    // QString hexData = ui->writeHexEdit->text().trimmed();
    // if (hexData.isEmpty()) {
    //     msgBox::show("错误","请输入十六进制数据，例如: 01 02 AB",1);
    //     return;
    // }
    // QByteArray bytes = hexStringToByteArray(hexData);
    // if (bytes.isEmpty()) {
    //     QMessageBox::warning(this, "错误", "无效的十六进制格式");
    //     return;
    // }
    // if (!currentService) {
    //     for (int i = 0; i < ui->serviceTree->topLevelItemCount(); ++i) {
    //         QTreeWidgetItem *svcItem = ui->serviceTree->topLevelItem(i);
    //         for (int j = 0; j < svcItem->childCount(); ++j) {
    //             QTreeWidgetItem *chItem = svcItem->child(j);
    //             if (chItem->data(0, Qt::UserRole).value<QLowEnergyCharacteristic>().uuid() == selectedCharacteristic.uuid()) {
    //                 currentService = svcItem->data(0, Qt::UserRole).value<QLowEnergyService*>();
    //                 break;
    //             }
    //         }
    //     }
    // }
    // if (currentService) {
    //     QLowEnergyService::WriteMode mode = (ui->writeModeCombo->currentIndex() == 0) ?
    //                 QLowEnergyService::WriteWithResponse : QLowEnergyService::WriteWithoutResponse;
    //     currentService->writeCharacteristic(selectedCharacteristic, bytes, mode);
    //     qDebug()<<QString("写入特征值 %1 : %2").arg(selectedCharacteristic.uuid().toString(),QString::fromLatin1(bytes.toHex(' ')));
    // }
}

void MainWindow::on_subscribeButton_clicked()
{
    if (!selectedCharacteristic.isValid()) {
        msgBox::show("错误","请先选中一个支持通知/指示的特征值",1);
        return;
    }
    if (!currentService) {
        for (int i = 0; i < ui->serviceTree->topLevelItemCount(); ++i) {
            QTreeWidgetItem *svcItem = ui->serviceTree->topLevelItem(i);
            for (int j = 0; j < svcItem->childCount(); ++j) {
                QTreeWidgetItem *chItem = svcItem->child(j);
                if (chItem->data(0, Qt::UserRole).value<QLowEnergyCharacteristic>().uuid() == selectedCharacteristic.uuid()) {
                    currentService = svcItem->data(0, Qt::UserRole).value<QLowEnergyService*>();
                    break;
                }
            }
        }
    }
    if (!currentService)
        return;

    if (!isSubscribed) {
        QLowEnergyDescriptor notificationDesc = selectedCharacteristic.descriptor(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration);
        if (!notificationDesc.isValid()) {
            qDebug()<<"错误: 该特征值没有客户端配置描述符(CCCD)，无法订阅";
            return;
        }
        const QByteArray enableDescValue = QByteArray::fromHex("0100");
        currentService->writeDescriptor(notificationDesc, enableDescValue);
        qDebug()<<"订阅通知中...";
        isSubscribed = true;
        ui->subscribeButton->setText("断开服务");
    } else {
        QLowEnergyDescriptor notificationDesc = selectedCharacteristic.descriptor(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration);
        if (notificationDesc.isValid()) {
            const QByteArray disableDescValue = QByteArray::fromHex("0000");
            currentService->writeDescriptor(notificationDesc, disableDescValue);
            qDebug()<<"取消订阅";
        }
        isSubscribed = false;
        ui->subscribeButton->setText("连接服务");
    }
}

void MainWindow::on_clearButton_clicked()
{
    ui->plotWidget->clearGraphs();
    ui->plotWidget->yAxis->setRange(0,90);
    ui->plotWidget->replot();
    ui->missionIDLabel->setText("");
    ui->detectResultLabel->setText("");
    ui->pressureLabel->setText("");
    ui->diameterLabel->setText("");
    ui->resultLabel->setText("");
}

void MainWindow::updateConnectionUI(bool connected)
{
    ui->scanButton->setEnabled(!connected);
    ui->connectButton->setEnabled(!connected);
    if(connected)
    {
        ui->connectButton->setText("断开连接");
        // 断开连接功能：点击后断开
        disconnect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::on_connectButton_clicked);
        connect(ui->connectButton, &QPushButton::clicked, this, [this](){
            if (controller)
                controller->disconnectFromDevice();
        });
    }
    else {
        ui->connectButton->setText("连接设备");
        disconnect(ui->connectButton, nullptr, this, nullptr);
        connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::on_connectButton_clicked);
    }
}

QByteArray MainWindow::hexStringToByteArray(const QString &hex)
{
    QString cleaned = hex;
    cleaned.remove(' ');
    cleaned.remove('\t');
    cleaned.remove('\n');
    if (cleaned.length() % 2 != 0)
        return QByteArray();
    QByteArray result;
    for (int i = 0; i < cleaned.length(); i += 2) {
        bool ok;
        char byte = static_cast<char>(cleaned.mid(i, 2).toUInt(&ok, 16));
        if (!ok)
            return QByteArray();
        result.append(byte);
    }
    return result;
}

void MainWindow::on_setPathButton_clicked()
{
    // 1. 加载上次选择的路径，若没有则默认为程序运行路径
    QSettings settings("MyCompany", "MyApp");
    QString lastFolderPath = settings.value("LastFolderPath", QCoreApplication::applicationDirPath()).toString();
    m_fileFolderPath = QFileDialog::getExistingDirectory(this,
                                                           tr("选择文件夹"),
                                                           lastFolderPath,
                                                           QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (m_fileFolderPath.isEmpty()) {
        m_fileFolderPath = QCoreApplication::applicationDirPath();
        settings.setValue("LastFolderPath", m_fileFolderPath);
        return; // 用户取消选择
    }
    settings.setValue("LastFolderPath", m_fileFolderPath);
}


void MainWindow::on_saveButton_clicked()
{
    ui->saveButton->setEnabled(false);
    if("" == ui->missionIDLabel->text())
    {
        qDebug()<<"导出失败，右侧无数据";
    }
    ui->saveButton->setEnabled(true);
    // 1. 加载上次选择的路径，若没有则默认为程序运行路径
    QSettings settings("MyCompany", "MyApp");
    QString lastFolderPath = settings.value("LastFolderPath", QCoreApplication::applicationDirPath()).toString();
    m_fileFolderPath = QFileDialog::getExistingDirectory(this,
                                                         tr("导出"),
                                                         lastFolderPath,
                                                         QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (m_fileFolderPath.isEmpty()) {
        m_fileFolderPath = lastFolderPath;
        settings.setValue("LastFolderPath", m_fileFolderPath);
        ui->saveButton->setEnabled(true);
        return; // 用户取消选择
    }
    settings.setValue("LastFolderPath", m_fileFolderPath);
    if(!exportPlotToImage(ui->plotWidget,513,512))
    {
        ui->saveButton->setEnabled(true);
        return;
    }
    QString plotImagePath = QCoreApplication::applicationDirPath()+"/"+ui->missionIDLabel->text()+".png";
    WordEngine wordEngine;
    QString templatePath = QCoreApplication::applicationDirPath() + "/template.doc";
    if (!wordEngine.openTemplate(templatePath)) {
        QMessageBox::information(this,"错误","无法打开模板，请确保已安装 Word 或 WPS");
        QFile::remove(plotImagePath);
        ui->saveButton->setEnabled(true);
        return;
    }
    // 替换书签
    wordEngine.replaceBookmarkText("business_tag", ui->missionIDLabel->text());
    wordEngine.replaceBookmarkText("detect_result",ui->detectResultLabel->text());
    wordEngine.replaceBookmarkText("detect_result1",ui->detectResultLabel->text());
    wordEngine.replaceBookmarkText("detect_result2",ui->detectResultLabel->text());
    wordEngine.replaceBookmarkText("diameter_value",ui->diameterLabel->text());
    wordEngine.replaceBookmarkText("pressure_value",ui->pressureLabel->text());
    wordEngine.replaceBookmarkText("test_result",ui->resultLabel->text());
    wordEngine.replaceBookmarkText("test_result1",ui->resultLabel->text());
    QString yesOrNo = ui->resultLabel->text()=="合格"?"符合":"不符合";
    wordEngine.replaceBookmarkText("yes_no",yesOrNo);
    // 插入图片
    wordEngine.insertImageAtBookmark("plot_image", plotImagePath, 0, 0);
    // 保存报告
    QString saveFileName = ui->missionIDLabel->text() + "_报告.doc";
    QString savePath = m_fileFolderPath + "/" + saveFileName;

    if (wordEngine.saveAs(savePath)) {
        wordEngine.close();
        QMessageBox::information(this,"成功",QString("报告已生成：%1\n使用应用：%2")
                                                   .arg(savePath).arg(wordEngine.currentApplicationName()));
    } else {
        QMessageBox::information(this, "错误", "保存报告失败");
    }
    QFile::remove(plotImagePath);
    ui->saveButton->setEnabled(true);
    // wordEngine 析构时自动关闭应用
}

