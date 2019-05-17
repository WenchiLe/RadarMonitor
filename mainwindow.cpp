#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->centralWidget->setMouseTracking(true);
    setMouseTracking(true);

    this->setFixedSize( this->width (),this->height ());

    getLicensePlateThread = new GetLicensePlateThread(&receiveDataFromServer);
    getFramesThread = new GetFramesThread(&receiveDataFromServer);

    connect(getFramesThread, SIGNAL(FramesChanged(ReceiveDataFromServer::Frame60Bs)),
            this, SLOT(NewFramesCome(ReceiveDataFromServer::Frame60Bs)));
    connect(getFramesThread, SIGNAL(ToStoreFrames(ReceiveDataFromServer::Frame60Bs)),
            &radarFrameProcessThread, SLOT(StoreNewFrames(ReceiveDataFromServer::Frame60Bs)));
    connect(getLicensePlateThread, SIGNAL(LicensePlateChanged(ReceiveDataFromServer::CarLicense)),
            &radarFrameProcessThread, SLOT(StoreLicensePlate(ReceiveDataFromServer::CarLicense)));

    lastFrame60Bs.length = 0;
    receiveDataFromServer.StartReceiveData();
    getLicensePlateThread->start();
    getFramesThread->start();
    radarFrameProcessThread.Start();

    point_location_pixmap_Car = QPoint(0,13);
    point_location_pixmap_Map = QPoint(713,476);
    point_location_video = QPoint(713,13);

    flag_translate_pixmap_Car = false;
    scale_pixmap_Car = 1;

    flag_translate_pixmap_Map = false;
    flag_can_send_radarID = false;
    radarID = 0;
    roadID = 0;
    scale_pixmap_Map = 1;
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

    //draw point in pixmap_Map
    QPen penMap;
    penMap.setColor(QColor(0, 0, 0,255));
    mapPainter.setPen(penMap);
    QBrush brushMap(QColor(0, 255, 0,255));
    mapPainter.setBrush(brushMap);
    int x = 10,y = 10;
    for(int j = 0; j < 4; j++)
    {
        double pointDis = sqrt(pow(cursorPointInPixMap.x()-x, 2) + pow(cursorPointInPixMap.y()-y, 2));
        if(pointDis <= 10)
        {
            brushMap.setColor(QColor(255, 0, 0, 255));
            mapPainter.setBrush(brushMap);
            if(flag_can_send_radarID)
            {
                if(j==3)
                {
                    roadID = 1;
                }else {
                    roadID = 0;
                }
                if(j != radarID)
                {
                    map_can_showDetail.clear();
                    if(j!=3)
                    {
                        radarID = j;
                        getFramesThread->SetRadarID(j);
                    }

                }
                //std::cout<<radarID<<std::endl;
                flag_can_send_radarID = false;
            }
        }
        mapPainter.drawEllipse(QPointF(x, y), 10, 10);
        mapPainter.drawText(x-2,y+3, QString::number(j));
        x += 30;
        brushMap.setColor(QColor(0, 255, 0, 255));
        mapPainter.setBrush(brushMap);
    }

    //draw the road in pixmap_Car
    QPixmap pix_Road;
    switch (roadID) {
    case 0:
        pix_Road.load("images/road.png");
        carPainter.drawPixmap(245,-100,209,1100,pix_Road);
        break;
    case 1:
        pix_Road.load("images/road4.png");
        carPainter.drawPixmap(265,-100,131,1100,pix_Road);
        break;
    }


    //draw the grid coordinate in pixmap_Car
    QPen penGrid;
    penGrid.setColor(QColor(0, 0, 0,50));
    carPainter.setPen(penGrid);
    int indexRow = 260;
    int indexColumn = 50;
    for(int j = 1 ;j<=14;j++)
    {
        carPainter.drawLine(QPointF(30, 60*j), QPointF(670, 60*j));//row
        carPainter.drawText(10,60*j,QString::number(indexRow));
        if(j==14) break;
        indexRow -=20;
    }
    for(int j = 1; j<=11;j++)
    {
        carPainter.drawLine(QPointF(50+60*(j-1), 10), QPointF(50+60*(j-1), 840));//column
        carPainter.drawText(50+60*(j-1),860,QString::number(indexColumn));
        //if(indexColumn == 0) std::cout<<50+60*(j-1)<<std::endl;
        indexColumn -= 10;
    }

    //draw the objects in pixmap_Car
    QPen pen;
    pen.setColor(QColor(0, 0, 0,255));
    carPainter.setPen(pen);
    QBrush brush(Qt::white);
    carPainter.setBrush(brush);

    //    QPoint cursorPointInPix;
    //    cursorPointInPix.setX((cursorPoint.x()-point_origin_pixmap_Car.x()-point_location_pixmap_Car.x())/scale_pixmap_Car);
    //    cursorPointInPix.setY((cursorPoint.y()-point_origin_pixmap_Car.y()-point_location_pixmap_Car.y())/scale_pixmap_Car);
    for(int j=0;j<lastFrame60Bs.length;j++)
    {
        int x = (int)(350 - (lastFrame60Bs.frameData[j][2]*6));
        int y = (int)(840 - lastFrame60Bs.frameData[j][1]*3);
        int objID = lastFrame60Bs.frameData[j][0];

        //QPoint pointDis = cursorPointInPix - QPoint(x,y);
        double pointDis = sqrt(pow(cursorPointInPixCar.x()-x, 2) + pow(cursorPointInPixCar.y()-y, 2));
        if (pointDis <= 10)
        {
            //            cout<<pointDis<<endl;
            //            cout<<"X: "<<x<<" Y: "<<y<<endl;
            //            cout<<"pointX: "<<cursorPointInPix.x()<<" pointY: "<<cursorPointInPix.y()<<endl;
            //license = radarFrameProcessThread.GetLicense(radarID,objID);
            if(map_can_showDetail.value(objID,false))
            {
                map_can_showDetail.insert(objID,false);
                cursorPointInPixCar.setX(-10000);
                cursorPointInPixCar.setY(-10000);
            }
            else
            {
                map_can_showDetail.insert(objID,true);
                cursorPointInPixCar.setX(-10000);
                cursorPointInPixCar.setY(-10000);
            }

            //            brush.setColor(QColor(255, 0, 0,255));
            //            carPainter.setBrush(brush);
        }
        //                pen.setColor(QColor(0, 0, 0,255));
        //                carPainter.setPen(pen); //
        QPixmap pix;
        pix.load("images/car.png");
        carPainter.drawPixmap(x-6,y-12,12,24,pix);

        //carPainter.drawEllipse(QPointF(x, y), 6, 6); //
        //carPainter.drawText(x+13,y+6, QString::number(velocity,'f',0)+"km/h");
        //carPainter.drawText(x+7,y-7, "ID:"+QString::number(lastFrame60Bs.frame[j][0],'f',0));
        //        brush.setColor(QColor(0, 255, 0,255));
        //        carPainter.setBrush(brush);
    }

    //draw detail
    for(int j=0;j<lastFrame60Bs.length;j++)
    {
        int objID = lastFrame60Bs.frameData[j][0];
        if(map_can_showDetail.value(objID,false))
        {
            int x = (int)(350 - (lastFrame60Bs.frameData[j][2]*6));
            int y = (int)(840 - lastFrame60Bs.frameData[j][1]*3);
            float velocity = qSqrt(qPow(lastFrame60Bs.frameData[j][3],2)+qPow(lastFrame60Bs.frameData[j][4],2))*3.6;
            QString license = radarFrameProcessThread.GetLicense(radarID,objID);
            float dis_long = lastFrame60Bs.frameData[j][1];
            float dis_lat = lastFrame60Bs.frameData[j][2];
            carPainter.drawRect(x+6,y-12,90,50);
            carPainter.drawText(x+11,y, license.replace("AKB*","沪A·"));
            carPainter.drawText(x+11,y+11, QString::number(velocity,'f',0)+"km/h");
            carPainter.drawText(x+11,y+22,"纵向坐标:"+QString::number(dis_long,'f',0)+"m");
            carPainter.drawText(x+11,y+34,"横向坐标:"+QString::number(dis_lat,'f',0)+"m");
        }
    }

    //draw the licenese plate
    switch (roadID) {
    case 0:
        if(radarID == 0)//the license plate is near the first radar
        {
            QPen penPlate(Qt::gray,5);
            carPainter.setPen(penPlate);
            carPainter.drawLine(QPointF(230, 750), QPointF(470, 750));
            penPlate.setColor(Qt::black);
            carPainter.setPen(penPlate);
            carPainter.setFont(QFont("times",24));
            carPainter.drawText(475,760, "卡口");
            for(int j = 1; j<=5;j++)
            {
                QPixmap pix_plate;
                pix_plate.load("images/plate_white.png");
                carPainter.drawPixmap(282+(j-1)*30,740,20,20,pix_plate);
            }
        }
        break;
    case 1:
        break;
    }


    //draw the pixmaps on screen
    QPainter painter(this);
    painter.drawPixmap(point_location_pixmap_Car.x(), point_location_pixmap_Car.y(), pixmap_Car);
    painter.drawPixmap(point_location_pixmap_Map.x(), point_location_pixmap_Map.y(), pixmap_Map);
    painter.drawPixmap(point_location_video.x(), point_location_video.y(), pixmap_Video);

    //other process after screen had been drawn
    cursorPointInPixCar.setX(-10000);
    cursorPointInPixCar.setY(-10000);
}

void MainWindow::NewFramesCome(ReceiveDataFromServer::Frame60Bs frame60Bs)
{
    //std::cout<<frame60Bs.length<<std::endl;
    lastFrame60Bs = frame60Bs;
    update();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton) //click the left button of mouse
    {
        //if(event->pos().x()<=(1570+pixmapLocation.x())&&event->pos().x()>=pixmapLocation.x()&&event->pos().y()<=(900+pixmapLocation.y())&&event->pos().y()>=pixmapLocation.y())
        if(flag_isIn_pixmap_Car)
        {
            lastPoint = event->pos();
            flag_translate_pixmap_Car = true;
            cursorPointInPixCar.setX((cursorPoint.x()-point_origin_pixmap_Car.x()-point_location_pixmap_Car.x())/scale_pixmap_Car);
            cursorPointInPixCar.setY((cursorPoint.y()-point_origin_pixmap_Car.y()-point_location_pixmap_Car.y())/scale_pixmap_Car);
        }else{
            flag_translate_pixmap_Car = false;
        }
        if(flag_isIn_pixmap_Map)
        {
            cursorPointInPixMap.setX((cursorPoint.x()-point_origin_pixmap_Map.x()-point_location_pixmap_Map.x())/scale_pixmap_Map);
            cursorPointInPixMap.setY((cursorPoint.y()-point_origin_pixmap_Map.y()-point_location_pixmap_Map.y())/scale_pixmap_Map);
            flag_can_send_radarID = true;
            update();
        }
        //cout<<"point: "<<lastPoint.x()<<" "<<lastPoint.y()<<endl;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    cursorPoint = event->pos();
    if(cursorPoint.x()<=(700+point_location_pixmap_Car.x())&&cursorPoint.x()>=point_location_pixmap_Car.x()&&cursorPoint.y()<=(900+point_location_pixmap_Car.y())&&cursorPoint.y()>point_location_pixmap_Car.y())
    {
        flag_isIn_pixmap_Car = true;
        //cout<<cursorPoint.x()<<" "<<cursorPoint.y()<<endl;
    }
    else if(cursorPoint.x()<=(637+point_location_pixmap_Map.x())&&cursorPoint.x()>=point_location_pixmap_Map.x()&&cursorPoint.y()<=(437+point_location_pixmap_Map.y())&&cursorPoint.y()>point_location_pixmap_Map.y())
    {
        flag_isIn_pixmap_Car = false;
        flag_isIn_pixmap_Map = true;
        //cout<<cursorPoint.x()<<" "<<cursorPoint.y()<<endl;
    }else
    {
        flag_isIn_pixmap_Car = false;
        flag_isIn_pixmap_Map = false;
    }
    if(event->buttons()&Qt::LeftButton) //click the left button of mouse while move the mouse
    {
        if(flag_translate_pixmap_Car)
        {
            endPoint = event->pos();
            point_origin_pixmap_Car.setX(point_origin_pixmap_Car.x()+(endPoint.x()-lastPoint.x()));
            point_origin_pixmap_Car.setY(point_origin_pixmap_Car.y()+(endPoint.y()-lastPoint.y()));
            update(); //draw the screen
            lastPoint = endPoint;
        }
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) //release the left button of mouse
    {
        if(flag_translate_pixmap_Car)
        {
            endPoint = event->pos();
            point_origin_pixmap_Car.setX(point_origin_pixmap_Car.x()+(endPoint.x()-lastPoint.x()));
            point_origin_pixmap_Car.setY(point_origin_pixmap_Car.y()+(endPoint.y()-lastPoint.y()));
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
    if(flag_isIn_pixmap_Car)
    {
        //cout<<wheelPoint.x()<<" "<<wheelPoint.y()<<endl;
        qreal scaleLast = scale_pixmap_Car;
        if(event->delta()>0)
        {
            scale_pixmap_Car *=1.2;
        }
        else{
            if(scale_pixmap_Car>0.5)
                scale_pixmap_Car /=1.2;
        }
        point_origin_pixmap_Car.setX((scaleLast-scale_pixmap_Car)*(wheelPoint.x()-point_location_pixmap_Car.x())/scaleLast+(scale_pixmap_Car*point_origin_pixmap_Car.x()/scaleLast));
        point_origin_pixmap_Car.setY((scaleLast-scale_pixmap_Car)*(wheelPoint.y()-point_location_pixmap_Car.y())/scaleLast+(scale_pixmap_Car*point_origin_pixmap_Car.y()/scaleLast));
        update();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //std::cout<<"forced to stop"<<std::endl;
    getLicensePlateThread->Stop();
    getFramesThread->Stop();
    radarFrameProcessThread.Stop();
}
