#ifndef GSTVIDEORECEIVER_H
#define GSTVIDEORECEIVER_H

#if defined KEEP_FOR_FUTUR_IMPROVEMENT

#include <gst/gst.h>

#include <QQuickPaintedItem>
#include <QImage>

class GstVideoReceiver : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QString endpoint READ endpoint WRITE setEndpoint NOTIFY endpointChanged)
public:


    /**
     * @brief Constructor
     */
    GstVideoReceiver();

    /**
     * @brief Destructor
     */
   ~GstVideoReceiver();

    /**
      * @brief update displayed frame
      * @param painter
      */
     void paint(QPainter *painter);

     /**
       * @brief GStreamerComponent::setLastFrame
       * set the new last frame à delete the old
       * @param lastFrame
       */
     void setLastFrame(QImage* lastFrame);

     void setEndpoint(QString value);

     QString endpoint();


     Q_INVOKABLE void start();

     Q_INVOKABLE void stop();


signals:

     /**
      * @brief triggered when the component receive a new frame
      * @param newFrame
      */
     void haveNewFrame(QImage* newFrame);

     void endpointChanged(QString endpoint);

public slots:

     /**
     * @brief onHaveNewFrame slot
     * called whe we have a new image
     * @param newFrame
     */
     void onHaveNewFrame(QImage*  newFrame);

private:
    // gstreamer pipeline
    GstElement* _pipeline;
    // the last frame receive with gstreamer
    QImage *_frame;

    bool _isStart;

    QString _endpoint;


};

#endif
#endif // GSTVIDEORECEIVER_H
