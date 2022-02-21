#include "sensearea.h"

SenseArea::SenseArea(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_AcceptTouchEvents, true);
    isThereWasUpdate = false;
}


bool SenseArea::event(QEvent *event)
{
    //qDebug() << event;
    switch(event->type())
    {
    case QEvent::TouchBegin:{
        QTouchEvent * te = static_cast<QTouchEvent *>(event);
        emit TouchRecived(te);
        break;
    }
    case QEvent::TouchUpdate:{
        isThereWasUpdate = true;
        QTouchEvent * te = static_cast<QTouchEvent *>(event);
        emit TouchRecived(te);
        break;
    }
    case QEvent::TouchEnd:{
        if(isThereWasUpdate){
            isThereWasUpdate = false;
        }
        else{
            emit ClickRecived("du");
        }
        break;
    }
    default:
        return QWidget::event(event);
    }
    qDebug() << "Out";
    return true;
}
