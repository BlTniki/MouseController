#include "scrollbar.h"

ScrollBar::ScrollBar(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_AcceptTouchEvents, true);
}

bool ScrollBar::event(QEvent *event)
{
    switch(event->type())
    {
    case QEvent::TouchBegin:{

    }
    case QEvent::TouchEnd:{

    }
    case QEvent::TouchUpdate:{

        QTouchEvent * te = static_cast<QTouchEvent *>(event);
        emit TouchRecived(te);
        break;
    }
    default:
        return QWidget::event(event);
    }
    return true;
}

