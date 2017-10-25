#ifndef POINTMAPVM_H
#define POINTMAPVM_H

#include <QObject>

#include <I2PropertyHelpers.h>

#include "model/iop/agentiopm.h"

class PointMapVM : public QObject
{
    Q_OBJECT

       // Name of agent using this input/output represent by a point map
       I2_QML_PROPERTY(QString, nameAgent)

       // Model of our agent Input / Output / Parameter
       // I2_QML_PROPERTY_DELETE_PROOF(AgentIOPM*, iopModel)

       // Geometry for the connector in the view
       // Position the center of the connector (Absolute coordinate)
       I2_QML_PROPERTY(QPointF, position)

public:
    /**
     * @brief Default constructor
     * @param nameAgent The name of the agent using this input/output represent by a point map
     * @param model the Iop model which will be represent by the point map view
     * @param parent
     */
    explicit PointMapVM(QString nameAgent, QObject *parent = nullptr);

   /**
    * @brief Destructor
    */
    ~PointMapVM();

};

QML_DECLARE_TYPE(PointMapVM)
#endif // POINTMAPVM_H
