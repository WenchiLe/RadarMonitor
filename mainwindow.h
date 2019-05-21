#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "filedownloader.h"

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
    ReceiveDataFromServer::Frame60Bs lastFrame60Bs;

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

    QMap<int, bool> map_can_showDetail;

    FileDownloader *m_pImgCtrl;
    QPixmap videoImage;

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);
    void closeEvent(QCloseEvent *event);

private slots:
    void NewFramesCome(ReceiveDataFromServer::Frame60Bs frame60Bs);
    void loadImage();
};

#endif // MAINWINDOW_H
