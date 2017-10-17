#ifndef POINTMAPVM_H
#define POINTMAPVM_H

#include <QObject>

#include <I2PropertyHelpers.h>

#include "model/iop/agentiopm.h"

class PointMapVM : public QObject
{
    Q_OBJECT

       // Name of agent using this input/output
       I2_QML_PROPERTY_READONLY(QString, nameAgent)

       // Model of our agent Input / Output / Parameter
       //TODOESTIA : Question à vincent : on garde une instace de class IOPM ou on utilise que les infos dont on a besoin (à priori que IOP name)
       //peut être le type
       I2_QML_PROPERTY(AgentIOPM*, modelM)

       // Geometry for the connector in the view
       // Abscisse of the center of the connector (Absolute coordinate)
       I2_QML_PROPERTY(qreal, x_center)

       // Ordinate of the center of the connector (Absolute coordinate)
       I2_QML_PROPERTY(qreal, y_center)

       // Radius of the connector
       I2_QML_PROPERTY(qreal, radius)

public:
    /**
     * @brief Default constructor
     * @param model
     * @param parent
     */
    explicit PointMapVM(AgentIOPM* model, QObject *parent = nullptr);

   /**
    * @brief Destructor
    */
    ~PointMapVM();

};

QML_DECLARE_TYPE(PointMapVM)
#endif // POINTMAPVM_H
