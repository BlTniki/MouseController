#ifndef SCROLLBAR_H
#define SCROLLBAR_H

#include <QDebug>
#include <QWidget>
#include <QEvent>
#include <QTouchEvent>

/*
 * This class are for object, that processing touch input for turn mouse weel
 */
class ScrollBar : public QWidget
{
    Q_OBJECT;
public:
    ScrollBar(QWidget *parent = nullptr);

protected:
    bool event(QEvent *event) override;

signals:
    void TouchRecived(QTouchEvent*);
};

#endif // SCROLLBAR_H
