#ifndef QMIDIMAPPER_H
#define QMIDIMAPPER_H

#include <QObject>
#include <QWidget>
#include "qmidimessage.h"

class QMidiMapper : public QObject
{
    Q_OBJECT
public:
    explicit QMidiMapper(QObject *parent = 0);
private:
    QWidget *_widgetToMap;
signals:

signals:

public slots:
    void setMappingState(bool value = true);
    void setWidget(QWidget *widget);
    void onMidiMessageReceive(QMidiMessage *message);
};

#endif // QMIDIMAPPER_H
