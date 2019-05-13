#ifndef SLIDER_H
#define SLIDER_H

#include "RangeSlider.h"

class Slider : public RangeSlider
{
    Q_OBJECT
public:
    Slider(QWidget* aParent = Q_NULLPTR):RangeSlider(aParent){}
    int GetValue() const;
    void SetValue(int aLowerValue);

protected:
    void paintEvent(QPaintEvent* aEvent) override;
    int validWidth();
    QRectF firstHandleRect();
};

#endif // SLIDER_H
