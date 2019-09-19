#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#define ROADNUM 2

#include <QMainWindow>
#include <QPainter>
#include <getframesthread.h>
#include <radarframeprocessthread.h>
#include <getlicenseplatethread.h>
#include <receivedatafromserver.h>
#include <QtMath>
#include <QMap>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QCloseEvent>
#include <QTimer>
#include <QDateTime>
#include <QMessageBox>
#include <QDebug>
#include "RangeSlider.h"
#include "slider.h"
#include "sendconfig.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "filedownloader.h"

#define FRAMESIZE 12816

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    GetFramesThread *getFramesThread;
    GetLicensePlateThread *getLicensePlateThread;
    RadarFrameProcessThread radarFrameProcessThread;
    FrameStructData lastFrame60Bs;

    ReceiveDataFromServer receiveDataFromServer;

    QPoint point_location_pixmap_Car;
    QPoint point_location_pixmap_Map;
    QPoint point_location_video;

    QPoint cursorPoint;
    QPoint wheelPoint;
    QPoint lastPoint;
    QPoint endPoint;

    QPoint point_origin_pixmap_Car;
    QPoint point_origin_pixmap_Map;

    QPoint cursorPointInPixMap;
    QPoint cursorPointInPixCar;

    bool flag_isIn_pixmap_Car;
    bool flag_translate_pixmap_Car;

    bool flag_isIn_pixmap_Map;
    bool flag_translate_pixmap_Map;
    bool flag_can_send_radarID;

    qreal scale_pixmap_Car;
    qreal scale_pixmap_Map;

    int radarID;
    int roadID;
    int roadOffSet[ROADNUM] = {0};
    int roadZoom[ROADNUM] = {0};

    QMap<int, bool> map_can_showDetail;

    FileDownloader *m_pImgCtrl;
    QPixmap videoImage;

    qreal carAngle = 0;

    RangeSlider *rangeSlider_long_dis;
    RangeSlider *rangeSlider_lat_dis;
    RangeSlider *rangeSlider_oncom_v;
    RangeSlider *rangeSlider_depart_v;

    Slider *Slider_angle;

    SendConfig sendConfig;

    QTimer *timer;

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);
    void closeEvent(QCloseEvent *event);

private slots:
    void NewFramesCome(FrameStructData frame60Bs);
    void on_pushButton_road_left_clicked();
    void on_pushButton_road_right_clicked();
    void on_pushButton_road_enlarge_clicked();
    void on_pushButton_road_shrink_clicked();
    void on_Btn_submit_clicked();
    void ReceiveSentConfigMsg(bool flag);
    void timerUpdate();
    void loadImage();

signals:
    void SetConfig(QVector<SendConfig::ScaleSetInfo> ScaleSetInfoS);
};

#endif // MAINWINDOW_H
