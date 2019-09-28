#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // check the frame size
    std::cout << "sizeof frame is " << sizeof(FrameStructData)  << std::endl;
    if (sizeof(FrameStructData) != FRAMESIZE)
    {
    std::cout << "sizeof frame is not " << FRAMESIZE  << std::endl;
    this->close();
    }
    ui->setupUi(this);

    ui->centralWidget->setMouseTracking(true);
    setMouseTracking(true);

    this->setFixedSize(this->width(), this->height());

    qRegisterMetaType<QVector<SendConfig::ScaleSetInfo>>("QVector<SendConfig::ScaleSetInfo>");
    qRegisterMetaType<SendConfig::ScaleSetInfo>("SendConfig::ScaleSetInfo");

    getLicensePlateThread = new GetLicensePlateThread(&receiveDataFromServer);
    getFramesThread = new GetFramesThread(&receiveDataFromServer);

    QUrl imageUrl("http://192.168.1.106:8080/cam_1.jpg");
    m_pImgCtrl = new FileDownloader(imageUrl);

    connect(getFramesThread, SIGNAL(FramesChanged(FrameStructData)),
        this, SLOT(NewFramesCome(FrameStructData)));
    connect(getFramesThread, SIGNAL(ToStoreFrames(FrameStructData)),
        &radarFrameProcessThread, SLOT(StoreNewFrames(FrameStructData)));
    connect(getLicensePlateThread, SIGNAL(LicensePlateChanged(ReceiveDataFromServer::CarLicense)),
        &radarFrameProcessThread, SLOT(StoreLicensePlate(ReceiveDataFromServer::CarLicense)));
    connect(m_pImgCtrl, SIGNAL(downloaded()), this, SLOT(loadImage()));


    connect(this, SIGNAL(SetConfig(QVector<SendConfig::ScaleSetInfo>)),
        &sendConfig, SLOT(GetConfig(QVector<SendConfig::ScaleSetInfo>)));
    connect(&sendConfig, SIGNAL(SendConfigMsg(bool)),
        this, SLOT(ReceiveSentConfigMsg(bool)));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    timer->start(1);

    lastFrame60Bs.length = 0;
    receiveDataFromServer.StartReceiveData();
    getLicensePlateThread->start();
    getFramesThread->start();
    radarFrameProcessThread.Start();
    m_pImgCtrl->start();

    point_location_pixmap_Car = QPoint(0, 13);
    point_location_pixmap_Map = QPoint(713, 476);
    point_location_video = QPoint(713, 13);

    flag_translate_pixmap_Car = false;
    scale_pixmap_Car = 1;

    flag_translate_pixmap_Map = false;
    flag_can_send_radarID = false;
    radarID = 0;
    roadID = 0;
    scale_pixmap_Map = 1;

    rangeSlider_long_dis = ui->rangeSlider_long_dis;
    rangeSlider_lat_dis = ui->rangeSlider_lat_dis;
    rangeSlider_oncom_v = ui->rangeSlider_oncom_v;
    rangeSlider_depart_v = ui->rangeSlider_depart_v;
    Slider_angle = ui->Slider_angle;

    rangeSlider_long_dis->SetMaximum(260);
    rangeSlider_long_dis->SetMinimum(0);
    rangeSlider_long_dis->SetUpperValue(260);
    rangeSlider_long_dis->SetLowerValue(0);

    rangeSlider_lat_dis->SetMaximum(260);
    rangeSlider_lat_dis->SetMinimum(0);
    rangeSlider_lat_dis->SetUpperValue(260);
    rangeSlider_lat_dis->SetLowerValue(0);

    rangeSlider_oncom_v->SetMaximum(260);
    rangeSlider_oncom_v->SetMinimum(0);
    rangeSlider_oncom_v->SetUpperValue(260);
    rangeSlider_oncom_v->SetLowerValue(0);

    rangeSlider_depart_v->SetMaximum(260);
    rangeSlider_depart_v->SetMinimum(0);
    rangeSlider_depart_v->SetUpperValue(260);
    rangeSlider_depart_v->SetLowerValue(0);

    Slider_angle->SetRange(-90, 90);
    Slider_angle->SetValue(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPixmap pixmap_Car;
    pixmap_Car = QPixmap(700, 900);
    pixmap_Car.fill(Qt::white);

    QPixmap pixmap_Map;
    pixmap_Map = QPixmap(637, 437);
    pixmap_Map.fill(Qt::white);

    QPixmap pixmap_Video;
    pixmap_Video = QPixmap(637, 450);
    pixmap_Video.fill(Qt::white);

    QPainter carPainter(&pixmap_Car);
    carPainter.translate(point_origin_pixmap_Car.x(), point_origin_pixmap_Car.y());
    carPainter.scale(scale_pixmap_Car, scale_pixmap_Car);

    QPainter mapPainter(&pixmap_Map);
    mapPainter.translate(point_origin_pixmap_Map.x(), point_origin_pixmap_Map.y());
    mapPainter.scale(scale_pixmap_Map, scale_pixmap_Map);

    QPainter videoPainter(&pixmap_Video);

    //draw point in pixmap_Map
//    QPen penMap;
//    penMap.setColor(QColor(0, 0, 0, 255));
//    mapPainter.setPen(penMap);
//    QBrush brushMap(QColor(0, 255, 0, 255));
//    mapPainter.setBrush(brushMap);
//    int x = 10, y = 10;
//    for (int j = 0; j < 4; j++)
//    {
//    double pointDis = sqrt(pow(cursorPointInPixMap.x() - x, 2) + pow(cursorPointInPixMap.y() - y, 2));
//    if (pointDis <= 10)
//    {
//        brushMap.setColor(QColor(255, 0, 0, 255));
//        mapPainter.setBrush(brushMap);
//        if (flag_can_send_radarID)
//        {
//        if (j == 3)
//        {
//            roadID = 1;
//        }
//        else
//        {
//            roadID = 0;
//        }
//        if (j != radarID)
//        {
//            map_can_showDetail.clear();
//            if (j != 3)
//            {
//            radarID = j;
//            getFramesThread->SetRadarID(j);
//            }

//        }
//        //std::cout<<radarID<<std::endl;
//        flag_can_send_radarID = false;
//        }
//    }
//    mapPainter.drawEllipse(QPointF(x, y), 10, 10);
//    mapPainter.drawText(x - 2, y + 3, QString::number(j));
//    x += 30;
//    brushMap.setColor(QColor(0, 255, 0, 255));
//    mapPainter.setBrush(brushMap);
//    }

    //draw radarOverlook in pixmap_Map
    QPixmap pix_radarOverlook;
    if (flag_can_send_radarID && flag_isIn_pixmap_Map)
    {
    if (cursorPointInPixMap.y() > pixmap_Map.height() / 2)
    {
        if (radarID != 0)
        {
        map_can_showDetail.clear();
        lastFrame60Bs.length = 0;
        radarID = 0;
        getFramesThread->SetRadarID(radarID);
        }
    }
    else
    {
        if (radarID != 1)
        {
        map_can_showDetail.clear();
        lastFrame60Bs.length = 0;
        radarID = 1;
        getFramesThread->SetRadarID(radarID);
        }
    }
    flag_can_send_radarID = false;
    }

    switch (radarID)
    {
    case 0:
    pix_radarOverlook.load("images/radarOverlook_0highlight.png");
    break;

    case 1:
    pix_radarOverlook.load("images/radarOverlook_1highlight.png");
    break;

    default:
    pix_radarOverlook.load("images/radarOverlook.png");
    break;
    }
    mapPainter.drawPixmap(118, 0, 401, 432, pix_radarOverlook);

    //draw time in pixmap_Video
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy.MM.dd hh:mm:ss ddd");
    videoPainter.setFont(QFont("times", 35));
    videoPainter.drawText(30, 200, current_date);

    //draw the road in pixmap_Car
    QPixmap pix_Road;
    switch (roadID)
    {
    case 0:
    pix_Road.load("images/road2.png");
    carPainter.drawPixmap(237 + roadOffSet[roadID], -100, 222 + roadZoom[roadID], 1100, pix_Road);
    break;
    case 1:
    pix_Road.load("images/road4.png");
    carPainter.drawPixmap(265 + roadOffSet[roadID], -100, 150 + roadZoom[roadID], 1100, pix_Road);
    break;
    }

    //draw the grid coordinate in pixmap_Car
    QPen penGrid;
    penGrid.setColor(QColor(0, 0, 0, 50));
    carPainter.setPen(penGrid);
    int indexRow = 260;
    //int indexColumn = 50;
    int indexColumn = 100;
    for (int j = 1 ; j <= 14; j++)
    {
    carPainter.drawLine(QPointF(30, 60 * j), QPointF(670, 60 * j)); //row
    carPainter.drawText(10, 60 * j, QString::number(indexRow));
    if (j == 14)
        break;
    indexRow -= 20;
    }
    for (int j = 1; j <= 11; j++)
    {
    carPainter.drawLine(QPointF(50 + 60 * (j - 1), 10), QPointF(50 + 60 * (j - 1), 840)); //column
    carPainter.drawText(50 + 60 * (j - 1), 860, QString::number(indexColumn));
    //if(indexColumn == 0) std::cout<<50+60*(j-1)<<std::endl;
    indexColumn -= 20;
    }

    //draw the objects in pixmap_Car
    carPainter.save();
    carPainter.translate(350, 840);
    qreal rotateAngleDegree = Slider_angle->GetValue();
    qreal rotateAngleRadian = qDegreesToRadians(rotateAngleDegree);
    carPainter.rotate(rotateAngleDegree);
    QPen pen;
    pen.setColor(QColor(0, 0, 0, 255));
    carPainter.setPen(pen);
    QBrush brush(Qt::white);
    carPainter.setBrush(brush);
    QPoint cursorPointInPixCar_trans;
    cursorPointInPixCar_trans.setX((cursorPointInPixCar.x() - 350)*qCos(rotateAngleRadian) + (cursorPointInPixCar.y() - 840)*qSin(rotateAngleRadian));
    cursorPointInPixCar_trans.setY((cursorPointInPixCar.y() - 840)*qCos(rotateAngleRadian) - (cursorPointInPixCar.x() - 350)*qSin(rotateAngleRadian));
    for (int j = 0; j < lastFrame60Bs.length; j++)
    {
    //        int x = (int)(350 - (lastFrame60Bs.frame[j][2]*6));
    //        int y = (int)(840 - lastFrame60Bs.frame[j][1]*3);
    //int x = (int)(-(lastFrame60Bs.frameData[j][2] * 6));
    int x = (int)(-(lastFrame60Bs.frameData[j].currInfo.distLat * 3));
    int y = (int)(-lastFrame60Bs.frameData[j].currInfo.distLong * 3);
    int objID = lastFrame60Bs.frameData[j].objId;

    //QPoint pointDis = cursorPointInPix - QPoint(x,y);
    double pointDis = sqrt(pow(cursorPointInPixCar_trans.x() - x, 2) + pow(cursorPointInPixCar_trans.y() - y, 2));
    if (pointDis <= 10)
    {
        if (map_can_showDetail.value(objID, false))
        {
        map_can_showDetail.insert(objID, false);
        cursorPointInPixCar.setX(-10000);
        cursorPointInPixCar.setY(-10000);
        }
        else
        {
        map_can_showDetail.insert(objID, true);
        cursorPointInPixCar.setX(-10000);
        cursorPointInPixCar.setY(-10000);
        }
    }
    //                pen.setColor(QColor(0, 0, 0,255));
    //                carPainter.setPen(pen); //
    QPixmap pix("images/car.png");
    QMatrix matrix;
    float vy = lastFrame60Bs.frameData[j].currInfo.velLong;
    float vx = -lastFrame60Bs.frameData[j].currInfo.velLat;
//    if (qAbs(vx) == 0)
//    {
//        if (vy >= 0)
//        {
//        carAngle = 0;
//        }
//        else
//        {
//        carAngle = 180;
//        }
//    }
//    else
//    {
//        if (vx > 0)
//        {
//        carAngle = 90 - qRadiansToDegrees(qAtan(vy / vx));
//        }
//        else
//        {
//        carAngle = -90 - qRadiansToDegrees(qAtan(vy / vx));
//        }
//        //            if(qAbs(carAngle)>80)
//        //            {
//        //                std::cout<<"vx: "<<vx<<" vy: "<<vy<<std::endl;
//        //                std::cout<<"carAngle: "<<carAngle<<std::endl;
//        //            }
//    }
    if (vy < 0)
    {
        carAngle = 180;
    }
    else
    {
        carAngle = 0;
    }
    matrix.rotate(carAngle);
    pix = pix.transformed(matrix, Qt::SmoothTransformation);
    carPainter.drawPixmap(x - 6, y - 13, pix);
    }

    //draw detail
    for (int j = 0; j < lastFrame60Bs.length; j++)
    {
    int objID = lastFrame60Bs.frameData[j].objId;
    //int x = (int)(-(lastFrame60Bs.frameData[j][2] * 6));
    int x = (int)(-(lastFrame60Bs.frameData[j].currInfo.distLat * 3));
    int y = (int)(-lastFrame60Bs.frameData[j].currInfo.distLong * 3);
    QString license = radarFrameProcessThread.GetLicense(radarID, objID);
    if (map_can_showDetail.value(objID, false))
    {
        //carPainter.setPen(pen);
        carPainter.setFont(QFont("times", 10));
        //            int x = (int)(350 - (lastFrame60Bs.frame[j][2]*6));
        //            int y = (int)(840 - lastFrame60Bs.frame[j][1]*3);
        float velocity = qSqrt(qPow(lastFrame60Bs.frameData[j].currInfo.velLong, 2) + qPow(lastFrame60Bs.frameData[j].currInfo.velLat, 2)) * 3.6;
        float dis_long = lastFrame60Bs.frameData[j].currInfo.distLong;
        float dis_lat = lastFrame60Bs.frameData[j].currInfo.distLat;
        carPainter.drawRect(x + 6, y - 12, 90, 50);
        //carPainter.drawText(x + 11, y, license.replace("AKB*", "沪A·"));
        carPainter.drawText(x + 11, y, QString::number(objID));
        carPainter.drawText(x + 11, y + 11, QString::number(velocity, 'f', 0) + "km/h");
        carPainter.drawText(x + 11, y + 22, "纵向坐标:" + QString::number(dis_long, 'f', 0) + "m");
        carPainter.drawText(x + 11, y + 34, "横向坐标:" + QString::number(dis_lat, 'f', 0) + "m");
    }

    carPainter.setFont(QFont("times", 38));
    if (x >= 0)
    {
        pen.setWidth(5);
        pen.setStyle(Qt::DashDotLine);
        carPainter.setPen(pen);
        carPainter.drawLine(QPointF(x + 8, y), QPointF(x + 200, y));
        pen.setWidth(1);
        pen.setStyle(Qt::SolidLine);
        carPainter.setPen(pen);
        carPainter.drawRect(x + 200, y - 24, 96, 48);
        carPainter.drawText(x + 209, y + 17, license);
    }
    else
    {
        pen.setWidth(5);
        pen.setStyle(Qt::DashDotLine);
        carPainter.setPen(pen);
        carPainter.drawLine(QPointF(x - 8, y), QPointF(x - 200, y));
        pen.setWidth(1);
        pen.setStyle(Qt::SolidLine);
        carPainter.setPen(pen);
        carPainter.drawRect(x - 296, y - 24, 96, 48);
        carPainter.drawText(x - 287, y + 17, license);
    }
    }

    //draw the licenese plate
    carPainter.restore();
//    switch (roadID)
//    {
//    case 0:
//    if (radarID == 0) //the license plate is near the first radar
//    {
//        QPen penPlate(Qt::gray, 5);
//        carPainter.setPen(penPlate);
//        carPainter.drawLine(QPointF(230, 750), QPointF(470, 750));
//        penPlate.setColor(Qt::black);
//        carPainter.setPen(penPlate);
//        carPainter.setFont(QFont("times", 24));
//        carPainter.drawText(475, 760, "卡口");
//        for (int j = 1; j <= 5; j++)
//        {
//        QPen penPlate(Qt::gray, 5);
//        carPainter.setPen(penPlate);
//        carPainter.drawLine(QPointF(230 + roadOffSet[roadID], 750), QPointF(470 + roadOffSet[roadID] + roadZoom[roadID], 750));
//        penPlate.setColor(Qt::black);
//        carPainter.setPen(penPlate);
//        carPainter.setFont(QFont("times", 24));
//        carPainter.drawText(475 + roadOffSet[roadID] + roadZoom[roadID], 760, "卡口");
//        for (int j = 1; j <= 5; j++)
//        {
//            QPixmap pix_plate;
//            pix_plate.load("images/plate_white.png");
//            carPainter.drawPixmap(282 + (j - 1) * (30 + roadZoom[roadID] / 7) + roadOffSet[roadID], 740, 20 + roadZoom[roadID], 20, pix_plate);
//        }
//        }
//    }
//    break;
//    case 1:
//    break;
//    }

    //draw the video
    videoPainter.drawPixmap(0, 0, 637, 450, videoImage);
    m_pImgCtrl->exit();

    //draw the pixmaps on screen
    QPainter painter(this);
    painter.drawPixmap(point_location_pixmap_Car.x(), point_location_pixmap_Car.y(), pixmap_Car);
    painter.drawPixmap(point_location_pixmap_Map.x(), point_location_pixmap_Map.y(), pixmap_Map);
    painter.drawPixmap(point_location_video.x(), point_location_video.y(), pixmap_Video);

    //other process after screen had been drawn
    cursorPointInPixCar.setX(-10000);
    cursorPointInPixCar.setY(-10000);
    //lastFrame60Bs.length = 0;

    //restart the timer
    timer->start();
}

void MainWindow::NewFramesCome(FrameStructData frame60Bs)
{
    //std::cout<<frame60Bs.length<<std::endl;
    lastFrame60Bs = frame60Bs;
    update();
}

void MainWindow::loadImage()
{
    //std::cout << "loadImage" << std::endl;
    videoImage.loadFromData(m_pImgCtrl->downloadedData());
    update();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) //click the left button of mouse
    {
    //if(event->pos().x()<=(1570+pixmapLocation.x())&&event->pos().x()>=pixmapLocation.x()&&event->pos().y()<=(900+pixmapLocation.y())&&event->pos().y()>=pixmapLocation.y())
    if (flag_isIn_pixmap_Car)
    {
        lastPoint = event->pos();
        flag_translate_pixmap_Car = true;
        cursorPointInPixCar.setX((cursorPoint.x() - point_origin_pixmap_Car.x() - point_location_pixmap_Car.x()) / scale_pixmap_Car);
        cursorPointInPixCar.setY((cursorPoint.y() - point_origin_pixmap_Car.y() - point_location_pixmap_Car.y()) / scale_pixmap_Car);
    }
    else
    {
        flag_translate_pixmap_Car = false;
    }
    if (flag_isIn_pixmap_Map)
    {
        cursorPointInPixMap.setX((cursorPoint.x() - point_origin_pixmap_Map.x() - point_location_pixmap_Map.x()) / scale_pixmap_Map);
        cursorPointInPixMap.setY((cursorPoint.y() - point_origin_pixmap_Map.y() - point_location_pixmap_Map.y()) / scale_pixmap_Map);
        flag_can_send_radarID = true;
        update();
    }
    //cout<<"point: "<<lastPoint.x()<<" "<<lastPoint.y()<<endl;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    cursorPoint = event->pos();
    if (cursorPoint.x() <= (700 + point_location_pixmap_Car.x()) && cursorPoint.x() >= point_location_pixmap_Car.x() && cursorPoint.y() <= (900 + point_location_pixmap_Car.y()) && cursorPoint.y() > point_location_pixmap_Car.y())
    {
    flag_isIn_pixmap_Car = true;
    flag_isIn_pixmap_Map = false;
    //cout<<cursorPoint.x()<<" "<<cursorPoint.y()<<endl;
    }
    else if (cursorPoint.x() <= (637 + point_location_pixmap_Map.x()) && cursorPoint.x() >= point_location_pixmap_Map.x() && cursorPoint.y() <= (437 + point_location_pixmap_Map.y()) && cursorPoint.y() > point_location_pixmap_Map.y())
    {
    flag_isIn_pixmap_Car = false;
    flag_isIn_pixmap_Map = true;
    //cout<<cursorPoint.x()<<" "<<cursorPoint.y()<<endl;
    }
    else
    {
    flag_isIn_pixmap_Car = false;
    flag_isIn_pixmap_Map = false;
    }
    if (event->buttons()&Qt::LeftButton) //click the left button of mouse while move the mouse
    {
    if (flag_translate_pixmap_Car)
    {
        endPoint = event->pos();
        point_origin_pixmap_Car.setX(point_origin_pixmap_Car.x() + (endPoint.x() - lastPoint.x()));
        point_origin_pixmap_Car.setY(point_origin_pixmap_Car.y() + (endPoint.y() - lastPoint.y()));
        update(); //draw the screen
        lastPoint = endPoint;
    }
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) //release the left button of mouse
    {
    if (flag_translate_pixmap_Car)
    {
        endPoint = event->pos();
        point_origin_pixmap_Car.setX(point_origin_pixmap_Car.x() + (endPoint.x() - lastPoint.x()));
        point_origin_pixmap_Car.setY(point_origin_pixmap_Car.y() + (endPoint.y() - lastPoint.y()));
        //cout<<"location: "<<lastPixLocalePoint.x()<<" "<<lastPixLocalePoint.y()<<endl;
        update(); ////draw the screen
        lastPoint = endPoint;
    }
    }
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    wheelPoint = event->pos();
    //if(wheelPoint.x()<=(1570+pixmapLocation.x())&&wheelPoint.x()>=pixmapLocation.x()&&wheelPoint.y()<=(900+pixmapLocation.y())&&wheelPoint.y()>=pixmapLocation.y())
    if (flag_isIn_pixmap_Car)
    {
    //cout<<wheelPoint.x()<<" "<<wheelPoint.y()<<endl;
    qreal scaleLast = scale_pixmap_Car;
    if (event->delta() > 0)
    {
        scale_pixmap_Car *= 1.2;
    }
    else
    {
        if (scale_pixmap_Car > 0.5)
        scale_pixmap_Car /= 1.2;
    }
    point_origin_pixmap_Car.setX((scaleLast - scale_pixmap_Car) * (wheelPoint.x() - point_location_pixmap_Car.x()) / scaleLast + (scale_pixmap_Car * point_origin_pixmap_Car.x() / scaleLast));
    point_origin_pixmap_Car.setY((scaleLast - scale_pixmap_Car) * (wheelPoint.y() - point_location_pixmap_Car.y()) / scaleLast + (scale_pixmap_Car * point_origin_pixmap_Car.y() / scaleLast));
    update();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //std::cout<<"forced to stop"<<std::endl;
    getLicensePlateThread->Stop();
    getFramesThread->Stop();
    radarFrameProcessThread.Stop();
    m_pImgCtrl->Stop();
}

void MainWindow::on_pushButton_road_left_clicked()
{
    roadOffSet[roadID]--;
    update();
}

void MainWindow::on_pushButton_road_right_clicked()
{
    roadOffSet[roadID]++;
    update();
}

void MainWindow::on_pushButton_road_enlarge_clicked()
{
    roadZoom[roadID]++;
    update();
}

void MainWindow::on_pushButton_road_shrink_clicked()
{
    roadZoom[roadID]--;
    update();
}

void MainWindow::on_Btn_submit_clicked()
{
    QVector<SendConfig::ScaleSetInfo> ScaleSetInfoS;
    float min;
    float max;
    min = rangeSlider_long_dis->GetLowerValue();
    max = rangeSlider_long_dis->GetUpperValue();

    SendConfig::ScaleSetInfo scaleSetInfo;
    scaleSetInfo.index = 0;
    scaleSetInfo.min = min;
    scaleSetInfo.max = max;
    ScaleSetInfoS.append(scaleSetInfo);


    min = rangeSlider_lat_dis->GetLowerValue();
    max = rangeSlider_lat_dis->GetUpperValue();

    scaleSetInfo.index = 1;
    scaleSetInfo.min = min;
    scaleSetInfo.max = max;
    ScaleSetInfoS.append(scaleSetInfo);

    min = rangeSlider_oncom_v->GetLowerValue();
    max = rangeSlider_oncom_v->GetUpperValue();

    scaleSetInfo.index = 2;
    scaleSetInfo.min = min;
    scaleSetInfo.max = max;
    ScaleSetInfoS.append(scaleSetInfo);

    min = rangeSlider_depart_v->GetLowerValue();
    max = rangeSlider_depart_v->GetUpperValue();

    scaleSetInfo.index = 3;
    scaleSetInfo.min = min;
    scaleSetInfo.max = max;
    ScaleSetInfoS.append(scaleSetInfo);

    min = Slider_angle->GetValue();
    max = Slider_angle->GetValue();

    scaleSetInfo.index = 4;
    scaleSetInfo.min = min;
    scaleSetInfo.max = max;
    ScaleSetInfoS.append(scaleSetInfo);

    getFramesThread->SetRadarAngle(Slider_angle->GetValue());
    Slider_angle->SetValue(0);

    //emit SetConfig(ScaleSetInfoS);
    //sendConfig.start();
}

void MainWindow::ReceiveSentConfigMsg(bool flag)
{
    if (flag)
    {
    QMessageBox::information(this, tr("设置信息发送状态"),
                 tr("设置信息发送成功"),
                 QMessageBox::Ok);
    }
    else
    {
    QMessageBox::critical(this, tr("设置信息发送状态"),
                  tr("设置信息发送失败"),
                  QMessageBox::Ok);
    }
}

void MainWindow::timerUpdate()
{
    update();
}
