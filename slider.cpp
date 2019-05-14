#include "slider.h"

void Slider::paintEvent(QPaintEvent* aEvent)
{
    Q_UNUSED(aEvent);
    QPainter painter(this);

    // Background
    QRectF backgroundRect = QRectF(scLeftRightMargin, (height() - scSliderBarHeight) / 2, width() - scLeftRightMargin * 2, scSliderBarHeight);
    QPen pen(Qt::gray, 0.8);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Qt4CompatiblePainting);
    QBrush backgroundBrush(QColor(0xD0, 0xD0, 0xD0));
    painter.setBrush(backgroundBrush);
    painter.drawRoundedRect(backgroundRect, 1, 1);

    // First value handle rect
    pen.setColor(Qt::darkGray);
    pen.setWidth(0.5);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing);
    QBrush handleBrush(QColor(0xFA, 0xFA, 0xFA));
    painter.setBrush(handleBrush);
    QRectF leftHandleRect = firstHandleRect();
    painter.drawRoundedRect(leftHandleRect, 2, 2);

    // Second value handle rect
//    QRectF rightHandleRect = secondHandleRect();
//    painter.drawRoundedRect(rightHandleRect, 2, 2);

    // Handles
    painter.setRenderHint(QPainter::Antialiasing, false);
    QRectF selectedRect(backgroundRect);
    selectedRect.setLeft(leftHandleRect.right() + 0.5);
//    selectedRect.setRight(rightHandleRect.left() - 0.5);
    QBrush selectedBrush(mBackgroudColor);
    painter.setBrush(selectedBrush);
    //painter.drawRect(selectedRect);

    // show the value
    pen.setColor(Qt::black);
    pen.setWidth(1);
    painter.setPen(pen);
    //painter.drawText(secondLocateX(),(height()-scHandleSideLength) / 2, QString::number(mUpperValue));
    painter.drawText(firstLocateX(),(height()-scHandleSideLength) / 2, QString::number(mLowerValue));

}

void Slider::mousePressEvent(QMouseEvent* aEvent)
{
    if(aEvent->buttons() & Qt::LeftButton)
    {
        //mSecondHandlePressed = secondHandleRect().contains(aEvent->pos());
        mFirstHandlePressed = !mSecondHandlePressed && firstHandleRect().contains(aEvent->pos());
        if(mFirstHandlePressed)
        {
            mDelta = aEvent->pos().x() - (firstHandleRect().x() + scHandleSideLength / 2);
        }
//        else if(mSecondHandlePressed)
//        {
//            mDelta = aEvent->pos().x() - (secondHandleRect().x() + scHandleSideLength / 2);
//        }
        if(aEvent->pos().y() >= 2
                && aEvent->pos().y() <= height()- 2)
        {
            int step = mInterval / 10 < 1 ? 1 : mInterval / 10;
            if(aEvent->pos().x() < firstHandleRect().x())
            {
                setLowerValue(mLowerValue - step);
            }
            else if(aEvent->pos().x() > firstHandleRect().x() + scHandleSideLength
                    && aEvent->pos().x() < secondHandleRect().x())
            {
                if(aEvent->pos().x() - (firstHandleRect().x() + scHandleSideLength) <
                        (secondHandleRect().x() - (firstHandleRect().x() + scHandleSideLength)) / 2)
                {
                    if(mLowerValue + step < mUpperValue)
                    {
                        setLowerValue(mLowerValue + step);
                    }
                    else
                    {
                        setLowerValue(mUpperValue);
                    }
                }
                else
                {
                    if(mUpperValue - step > mLowerValue)
                    {
                        setUpperValue(mUpperValue - step);
                    }
                    else
                    {
                        setUpperValue(mLowerValue);
                    }
                }
            }
            else if(aEvent->pos().x() > secondHandleRect().x() + scHandleSideLength)
            {
                setUpperValue(mUpperValue + step);
            }
        }
    }
}


QRectF Slider::firstHandleRect()
{
    float percentage = (mLowerValue - mMinimum) * 1.0 / mInterval;
    return handleRect(percentage * validWidth() + scLeftRightMargin);
}

int Slider::validWidth()
{
    return width() - scLeftRightMargin * 2 - scHandleSideLength * 1;
}

int Slider::GetValue() const
{
    return mLowerValue;
}

void Slider::SetValue(int aLowerValue)
{
    setLowerValue(aLowerValue);
}
