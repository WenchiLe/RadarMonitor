#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <getframesthread.h>
#include <radarframeprocessthread.h>
#include <getlicenseplatethread.h>
#include <QtMath>
#include <QMouseEvent>
#include <QWheelEvent>

namespace Ui {
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

    GetFramesThread getFramesThread;
    GetLicensePlateThread getLicensePlateThread;
    RadarFrameProcessThread radarFrameProcessThread;
    ReceiveData::Frame60Bs lastFrame60Bs;

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

    bool flag_isIn_pixmap_Car;
    bool flag_translate_pixmap_Car;

    bool flag_isIn_pixmap_Map;
    bool flag_translate_pixmap_Map;
    bool flag_can_send_radarID;

    qreal scale_pixmap_Car;
    qreal scale_pixmap_Map;

    int radarID;
    QString license;

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);

private slots:
    void NewFramesCome(ReceiveData::Frame60Bs frame60Bs);
};

#endif // MAINWINDOW_H
