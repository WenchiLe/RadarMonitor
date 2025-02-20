#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <getframesthread.h>
#include <radarframeprocessthread.h>
#include <getlicenseplatethread.h>
#include <QtMath>
#include <QMap>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QCloseEvent>

#define ROADNUM 2

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

    GetFramesThread getFramesThread;
    GetLicensePlateThread getLicensePlateThread;
    RadarFrameProcessThread radarFrameProcessThread;
    FrameStructData lastFrame60Bs;

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

    qreal carAngle = 0;

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
};

#endif // MAINWINDOW_H
