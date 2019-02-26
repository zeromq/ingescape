#include "GstVideoReceiver.h"

#if defined KEEP_FOR_FUTUR_IMPROVEMENT
#include <QApplication>
#include <QImage>
#include <QDebug>
#include <QPainter>
#include <QImage>

#include <gst/video/videooverlay.h>
#include <gst/video/video.h>
#include <gst/base/gstbasesink.h>
#include <gst/gst.h>



/**
 * @brief gstreamer callback when a new sample occur
 * @param gstappsink
 * @param gstreamerComponent here is the component reference
 * @return
 */
GstFlowReturn new_sample_cb(GstElement *gstappsink, void* gstreamerComponent)
{
    GstVideoReceiver* component = (GstVideoReceiver*) gstreamerComponent;
    GstSample *sample =  gst_base_sink_get_last_sample(GST_BASE_SINK(gstappsink));

    if (sample)
    {
        GstBuffer *buffer = gst_sample_get_buffer(sample);
        GstCaps   *caps   = gst_sample_get_caps(sample);

        GstVideoInfo info;
        GstMapInfo map;

        gst_video_info_from_caps( &info, caps) ;
        gst_buffer_map (buffer, &map, GST_MAP_READ);

        QImage::Format imageFormat = QImage::Format_RGB888;

        // transform sample to Qt image
        QImage *image = new QImage(map.data, info.width, info.height, imageFormat);

        // free useless data
        gst_buffer_unmap (buffer, &map);
        gst_buffer_unref(buffer);
        gst_sample_unref (sample);

        // emit the new image
        emit component->haveNewFrame(image);

        return GST_FLOW_OK;
    }
    else
    {
        return GST_FLOW_ERROR;
    }
}

/**
  * @brief update displayed frame
  * @param painter
  */
void GstVideoReceiver::paint(QPainter *painter)
{
    if(_frame != NULL)
    {
        // computing ratios in order not to change aspect ratio of streamed image
        qreal imageRatio = (qreal)_frame->width()/(qreal)_frame->height();
        qreal displayRatio = width() / height();

        // default case: the image ratio is higher than the display ratio:
        // take the full width of the display and adapt the height accordingly
        qreal widthToApply = width();
        qreal heightToApply = widthToApply/imageRatio;
        qreal xToApply = 0;
        qreal yToApply = (height()-heightToApply) / 2;

        // the display has a higher ratio: take the full height and adapt the width accordingly
        if(displayRatio > imageRatio)
        {
            heightToApply = height();
            widthToApply = heightToApply*imageRatio;
            xToApply = (width()-widthToApply) / 2;
            yToApply = 0;
        }

        // build our final zone where the image will be painted
        QRectF rect(xToApply, yToApply, widthToApply, heightToApply);
        painter->drawImage(rect, *_frame);
    }
}

/**
 * @brief Constructor
 */
GstVideoReceiver::GstVideoReceiver():QQuickPaintedItem (),_frame(NULL)
{


}


/**
 * @brief Destructor
 */
GstVideoReceiver::~GstVideoReceiver()
{
    stop();
}


/**
  * @brief setLastFrame
  * set the new last frame à delete the old
  * @param lastFrame
  */
void  GstVideoReceiver::setLastFrame(QImage* lastFrame)
{
    if(_frame != NULL)
    {
        delete _frame ;
    }
    _frame = lastFrame;
}

/**
 * @brief onHaveNewFrame slot
 * called whe we have a new image
 * @param newFrame
 */
void GstVideoReceiver::onHaveNewFrame(QImage* newFrame)
{
    if(_frame != newFrame)
    {
        setLastFrame(newFrame);
        update();
    }
}

void GstVideoReceiver::setEndpoint(QString value)
{
    if(value != _endpoint)
    {
        _endpoint = value;
        endpointChanged(value);
    }
}

QString GstVideoReceiver::endpoint()
{
    return _endpoint;
}


void GstVideoReceiver::start()
{
    if(! _isStart)
    {
        _isStart = true;
        connect(this,SIGNAL(haveNewFrame(QImage*)),this,SLOT(onHaveNewFrame(QImage*)));

        /* Build pipeline */

        GstElement *source = gst_element_factory_make ("zmqsrc", "source");
        if(!source){  g_printerr ("Cannot create zmqsrc !\n"); }

        GstElement *h264parse = gst_element_factory_make ("h264parse", "h264parse");
        if(!h264parse){  g_printerr ("Cannot create h264parse !\n"); }

        GstElement *avdec_h264 = gst_element_factory_make ("avdec_h264", "avdec_h264");
        if(!avdec_h264){  g_printerr ("Cannot create avdec_h264 !\n"); }

        GstElement *conv = gst_element_factory_make("videoconvert", "conv");
        if(!conv){  g_printerr ("Cannot create conv !\n"); }

        GstElement *filter = gst_element_factory_make ("capsfilter", "filter");

        GstCaps *caps = gst_caps_new_simple ("video/x-raw", "format",G_TYPE_STRING,"RGB", NULL);

        GstElement *appsink = gst_element_factory_make("appsink", NULL);

        if(!appsink){  g_printerr ("Cannot create appsink !\n"); }

        ////////////////////////////////////
        //     Create and link pipeline
        ////////////////////////////////////

        /* Create the empty pipeline */
        _pipeline = gst_pipeline_new ("pipeline");

        /* Build the pipeline */
        gst_bin_add_many (GST_BIN (_pipeline), source,  h264parse, avdec_h264, conv, filter, appsink, NULL);

        if (gst_element_link_many(source, h264parse, avdec_h264 ,conv,  filter, appsink, NULL) != TRUE)
        {
            g_printerr ("Elements could not be linked.\n");
            gst_object_unref (_pipeline);
        }

        //** configure endpoint zmq **/
        g_object_set(G_OBJECT(source), "endpoint", _endpoint.toStdString().c_str(), NULL);
        g_object_set(G_OBJECT(appsink), "emit-signals", TRUE, NULL);
        g_object_set (G_OBJECT (filter), "caps", caps, NULL);
        gst_caps_unref (caps);
        g_signal_connect(appsink, "new-sample", G_CALLBACK(new_sample_cb), this);

        /* run the pipeline */
        GstStateChangeReturn sret = gst_element_set_state (_pipeline, GST_STATE_PLAYING);
        if (sret == GST_STATE_CHANGE_FAILURE)
        {
            gst_element_set_state (_pipeline, GST_STATE_NULL);
            gst_object_unref (_pipeline);
        }
    }

}

void GstVideoReceiver::stop()
{
    if(_isStart)
    {
        disconnect(this, 0, this, 0);
        if(_frame != NULL)
        {
            delete _frame ;
            _frame = NULL;
        }
        gst_element_set_state (_pipeline, GST_STATE_NULL);
        gst_object_unref (_pipeline);
        _isStart = 0;
    }
}

#endif