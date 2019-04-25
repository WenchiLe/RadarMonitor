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

    connect(&getFramesThread, SIGNAL(FramesChanged(ReceiveData::Frame60Bs)),
            this, SLOT(NewFramesCome(ReceiveData::Frame60Bs)));
    connect(&getFramesThread, SIGNAL(ToStoreFrames(ReceiveData::Frame60Bs)),
            &radarFrameProcessThread, SLOT(StoreNewFrames(ReceiveData::Frame60Bs)));
    connect(&getLicensePlateThread, SIGNAL(LicensePlateChanged(ReceiveLicensePlate::carLicense)),
            &radarFrameProcessThread, SLOT(StoreLicensePlate(ReceiveLicensePlate::carLicense)));

    lastFrame60Bs.length = 0;
    getLicensePlateThread.start();
    getFramesThread.start();
    radarFrameProcessThread.start();

    point_location_pixmap_Car = QPoint(0,13);
    point_location_pixmap_Map = QPoint(713,476);
    point_location_video = QPoint(713,13);

    flag_translate_pixmap_Car = false;
    scale_pixmap_Car = 1;

    flag_translate_pixmap_Map = false;
    flag_can_send_radarID = false;
    radarID = 0;
    scale_pixmap_Map = 1;

    license = "";
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

    //draw the grid coordinate in pixmap_Car
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
    QBrush brush(QColor(0, 255, 0,255));
    carPainter.setBrush(brush);

    QPoint cursorPointInPix;
    cursorPointInPix.setX((cursorPoint.x()-point_origin_pixmap_Car.x()-point_location_pixmap_Car.x())/scale_pixmap_Car);
    cursorPointInPix.setY((cursorPoint.y()-point_origin_pixmap_Car.y()-point_location_pixmap_Car.y())/scale_pixmap_Car);
    for(int j=0;j<lastFrame60Bs.length;j++)
    {
        int x = (int)(350 - (lastFrame60Bs.frame[j][2]*6));
        int y = (int)(840 - lastFrame60Bs.frame[j][1]*3);
        //QPoint pointDis = cursorPointInPix - QPoint(x,y);
        double pointDis = sqrt(pow(cursorPointInPix.x()-x, 2) + pow(cursorPointInPix.y()-y, 2));
        if (pointDis <= 10)
        {
//            cout<<pointDis<<endl;
//            cout<<"X: "<<x<<" Y: "<<y<<endl;
//            cout<<"pointX: "<<cursorPointInPix.x()<<" pointY: "<<cursorPointInPix.y()<<endl;
            int objID = lastFrame60Bs.frame[j][0];
            license = radarFrameProcessThread.GetLicense(radarID,objID);
            //license = "hello"+QString::number(objID);
            brush.setColor(QColor(255, 0, 0,255));
            carPainter.setBrush(brush);
        }
        float velocity = qSqrt(qPow(lastFrame60Bs.frame[j][3],2)+qPow(lastFrame60Bs.frame[j][4],2))*3.6;
        //                pen.setColor(QColor(0, 0, 0,255));
        //                carPainter.setPen(pen); //
        carPainter.drawEllipse(QPointF(x, y), 6, 6); //
        //carPainter.drawText(x+13,y+6, QString::number(velocity,'f',0)+"km/h");
        carPainter.drawText(x+7,y-7, "ID:"+QString::number(lastFrame60Bs.frame[j][0],'f',0));
        brush.setColor(QColor(0, 255, 0,255));
        carPainter.setBrush(brush);
    }

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
                radarID = j;
                //std::cout<<radarID<<std::endl;
                getFramesThread.SetRadarID(j);
                flag_can_send_radarID = false;
            }
        }
        mapPainter.drawEllipse(QPointF(x, y), 10, 10);
        mapPainter.drawText(x-2,y+3, QString::number(j));
        x += 30;
        brushMap.setColor(QColor(0, 255, 0, 255));
        mapPainter.setBrush(brushMap);
    }

    mapPainter.drawText(10,40, license);

    //draw the pixmaps on screen
    QPainter painter(this);
    painter.drawPixmap(point_location_pixmap_Car.x(), point_location_pixmap_Car.y(), pixmap_Car);
    painter.drawPixmap(point_location_pixmap_Map.x(), point_location_pixmap_Map.y(), pixmap_Map);
    painter.drawPixmap(point_location_video.x(), point_location_video.y(), pixmap_Video);
}

void MainWindow::NewFramesCome(ReceiveData::Frame60Bs frame60Bs)
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
