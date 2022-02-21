#ifndef SENSEAREA_H
#define SENSEAREA_H

#include <QDebug>
#include <QWidget>
#include <QEvent>
#include <QEventLoop>
#include <QTouchEvent>

/*
 * This class are for object, that processing touch input for change mouse position
 */
class SenseArea : public QWidget
{
    Q_OBJECT;
public:
    SenseArea(QWidget *parent = nullptr);

private:
    bool isThereWasUpdate;// it's for implementation click on senseArea

protected:
    bool event(QEvent *event) override;

signals:
    void TouchRecived(QTouchEvent*);
    void ClickRecived(QString);
};

#endif // SENSEAREA_H
