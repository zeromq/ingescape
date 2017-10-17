#ifndef AGENTINMAPPINGVM_H
#define AGENTINMAPPINGVM_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>

#include <viewModel/pointmapvm.h>
#include <model/agentmappingm.h>

class AgentInMappingVM : public QObject
{
    Q_OBJECT

    // Name of our agent
    I2_QML_PROPERTY_READONLY(QString, name)

    // List of models of mapping agents
    I2_QOBJECT_LISTMODEL(AgentMappingM, models)

    // List of VM of inputs
    I2_QOBJECT_LISTMODEL(PointMapVM, inputsList)

    // List of VM of outputs
    I2_QOBJECT_LISTMODEL(PointMapVM, outputsList)

    // Geometry for the connector in the view
    // Abscisse of the center of the box (Absolute coordinate)
    I2_QML_PROPERTY(qreal, x_center)

    // Ordinate of the center of the box (Absolute coordinate)
    I2_QML_PROPERTY(qreal, y_center)

    // Width of the box
    I2_QML_PROPERTY(int, width)

    // Height of the box
    I2_QML_PROPERTY(int, height)


public:
    /**
         * @brief Default constructor
         * @param model
         * @param parent
         */
    explicit AgentInMappingVM(AgentMappingM* model, QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~AgentInMappingVM();

private:
    // Previous list of models of agents
    QList<AgentMappingM*> _previousAgentsMappingList;


};

QML_DECLARE_TYPE(AgentInMappingVM)

#endif // AGENTINMAPPINGVM_H
