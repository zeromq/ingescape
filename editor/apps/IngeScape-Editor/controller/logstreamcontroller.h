#ifndef LOGSTREAMCONTROLLER_H
#define LOGSTREAMCONTROLLER_H

#include <QObject>

class LogStreamController : public QObject
{
    Q_OBJECT
public:
    explicit LogStreamController(QObject *parent = nullptr);

signals:

public slots:
};

#endif // LOGSTREAMCONTROLLER_H