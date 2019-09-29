#include "datatable.h"
#include "ui_datatable.h"

DataTable::DataTable(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataTable)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
}

void DataTable::NewFramesCome(FrameStructData frame60Bs)
{
    //std::cout << "table :" << frame60Bs.length << std::endl;
    lastFrame60Bs = frame60Bs;
    if (flag_record)
    {
    queueFrame60Bs.enqueue(lastFrame60Bs);
    }
    if (lastFrame60Bs.length > 0)
    {
    update();
    }

}

void DataTable::paintEvent(QPaintEvent *)
{
    ui->dataTable->clearContents();
    int rowCount = ui->dataTable->rowCount();
    for (int j = 0; j < lastFrame60Bs.length && j < rowCount; j++)
    {
    ui->dataTable->setItem(j, 0, new QTableWidgetItem(QString::number(lastFrame60Bs.timeStamp)));
    ui->dataTable->setItem(j, 1, new QTableWidgetItem(QString::number(lastFrame60Bs.frameData[j].objId)));
    ui->dataTable->setItem(j, 2, new QTableWidgetItem(QString::number(lastFrame60Bs.frameData[j].histTraceInfo.currFrameIndex)));
    ui->dataTable->setItem(j, 3, new QTableWidgetItem(QString::number(1)));
    ui->dataTable->setItem(j, 4, new QTableWidgetItem(QString::number(lastFrame60Bs.frameData[j].currInfo.distLong)));
    ui->dataTable->setItem(j, 5, new QTableWidgetItem(QString::number(lastFrame60Bs.frameData[j].currInfo.distLat)));
    ui->dataTable->setItem(j, 6, new QTableWidgetItem(QString::number(lastFrame60Bs.frameData[j].objProperty.length)));
    ui->dataTable->setItem(j, 7, new QTableWidgetItem(QString::number(lastFrame60Bs.frameData[j].objProperty.width)));
    ui->dataTable->setItem(j, 8, new QTableWidgetItem(QString::number(lastFrame60Bs.frameData[j].objProperty.objClass)));
    float velocity = qSqrt(qPow(lastFrame60Bs.frameData[j].currInfo.velLong, 2) + qPow(lastFrame60Bs.frameData[j].currInfo.velLat, 2));
    ui->dataTable->setItem(j, 9, new QTableWidgetItem(QString::number(velocity)));
    ui->dataTable->setItem(j, 10, new QTableWidgetItem(QString::number(lastFrame60Bs.frameData[j].currInfo.acc)));
    //ui->dataTable->setItem(j, 11, new QTableWidgetItem(QString::number(lastFrame60Bs.frameData[j].objId)));
    //ui->dataTable->setItem(j, 12, new QTableWidgetItem(QString::number(lastFrame60Bs.frameData[j].objId)));
    ui->dataTable->setItem(j, 13, new QTableWidgetItem(QString::number(lastFrame60Bs.frameData[j].histTraceInfo.movement)));
    }
}

DataTable::~DataTable()
{
    delete ui;
}

void DataTable::on_Btn_start_record_data_clicked()
{
    flag_record = true;
    ui->Btn_stop_record_data->setEnabled(true);
    ui->Btn_start_record_data->setEnabled(false);
    ui->Btn_dataTable_return->setEnabled(false);
}

void DataTable::on_Btn_stop_record_data_clicked()
{
    flag_record = false;
    WriteCsv();
    queueFrame60Bs.clear();
    ui->Btn_stop_record_data->setEnabled(false);
    ui->Btn_start_record_data->setEnabled(true);
    ui->Btn_dataTable_return->setEnabled(true);
}

void DataTable::WriteCsv()
{
    std::ofstream outFile;
    std::string filename = "data_";
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy-MM-dd_hh-mm-ss");
    filename += current_date.toStdString();
    filename += ".csv";
    //std::cout << filename << std::endl;
    outFile.open(filename, std::ios::out); // 打开模式可省略
    outFile << "Timestamp" << ',' << "Vehicle_ID" << ',' << "Frame_ID" << ',' << "Total_Frames" << ',' << "Local_X" << ',' << "Local_Y" << ',' << "v_Length" << "," << "v_Width" << ',' << "v_Class" << ',' << "v_Vel" << ',' << "v_Acc" << ',' << "O_Zone" << ',' << "D_Zone" << ',' << "Movement" << std::endl;
    while (queueFrame60Bs.count() > 0)
    {
    FrameStructData frame60Bs = queueFrame60Bs.dequeue();
    for (int j = 0; j < frame60Bs.length; j++)
    {
        float velocity = qSqrt(qPow(frame60Bs.frameData[j].currInfo.velLong, 2) + qPow(frame60Bs.frameData[j].currInfo.velLat, 2));
        outFile << QString::number(frame60Bs.timeStamp).toStdString() << ','
            << QString::number(frame60Bs.frameData[j].objId).toStdString() << ','
            << QString::number(frame60Bs.frameData[j].histTraceInfo.currFrameIndex).toStdString() << ','
            << "1" << ','
            << QString::number(frame60Bs.frameData[j].currInfo.distLong).toStdString() << ','
            << QString::number(frame60Bs.frameData[j].currInfo.distLat).toStdString() << ','
            << QString::number(frame60Bs.frameData[j].objProperty.length).toStdString() << ","
            << QString::number(frame60Bs.frameData[j].objProperty.width).toStdString() << ','
            << QString::number(frame60Bs.frameData[j].objProperty.objClass).toStdString() << ','
            << QString::number(velocity).toStdString() << ','
            << QString::number(frame60Bs.frameData[j].currInfo.acc).toStdString() << ','
            << "" << ','
            << "" << ','
            << QString::number(frame60Bs.frameData[j].histTraceInfo.movement).toStdString()
            << std::endl;
    }
    }

    outFile.close();
    QString message = "文件保存为：";
    message += QString::fromStdString(filename);
    QMessageBox::information(this, tr("保存数据成功"),
                 message,
                 QMessageBox::Ok);

}
