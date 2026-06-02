#ifndef MYCUSTOMPLOT_H
#define MYCUSTOMPLOT_H

#include "qcustomplot.h"

class MyCustomPlot : public QCustomPlot
{
    Q_OBJECT
public:
    MyCustomPlot(QWidget *parent = nullptr);
    virtual ~MyCustomPlot() override;

signals:

public slots:
};

#endif // MYCUSTOMPLOT_H
