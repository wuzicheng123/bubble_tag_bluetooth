#ifndef DEFINE_H
#define DEFINE_H

#include <QByteArray>
#include <QStringList>

struct BleDataFrame {
    bool valid;                 // 是否解析成功
    QByteArray idx;             // 3字节IDX
    quint32 timestamp;          // 秒时间戳
    quint8 pressType;           // 压力类型
    quint8 pipType;             // Pip类型
    QByteArray bubbleData;      // 60字节气泡数据
    quint8 testResult;          // 测试结果 0不合格 1合格
    quint8 sum;                 // 校验和
};

#endif // DEFINE_H
