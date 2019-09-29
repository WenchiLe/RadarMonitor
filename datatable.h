#ifndef DATATABLE_H
#define DATATABLE_H

#include <QDialog>
#include <QTableWidget>
#include <QtMath>
#include <QQueue>
#include <QFile>
#include <QDateTime>
#include <QMessageBox>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "basestruct.h"

namespace Ui
{
class DataTable;
}

class DataTable : public QDialog
{
    Q_OBJECT

public:
    explicit DataTable(QWidget *parent = nullptr);
    ~DataTable();

private:
    Ui::DataTable *ui;
    FrameStructData lastFrame60Bs;
    QQueue<FrameStructData> queueFrame60Bs;
    bool flag_record = false;

    void WriteCsv();


protected:
    void paintEvent(QPaintEvent *);

private slots:
    void NewFramesCome(FrameStructData frame60Bs);

    void on_Btn_start_record_data_clicked();
    void on_Btn_stop_record_data_clicked();
};

#endif // DATATABLE_H
