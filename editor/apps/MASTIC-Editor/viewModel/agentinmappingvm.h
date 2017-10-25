#ifndef AGENTINMAPPINGVM_H
#define AGENTINMAPPINGVM_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>

#include <viewModel/pointmapvm.h>
#include <model/agentmappingm.h>
#include <model/definitionm.h>

class AgentInMappingVM : public QObject
{
    Q_OBJECT

    // Name of our agent
    I2_QML_PROPERTY_READONLY(QString, agentName)

    // List of models of definition agents
    I2_QOBJECT_LISTMODEL(DefinitionM, definitionModelList)

    //TODO ESTIA input output VM
    // List of VM of inputs
    I2_QOBJECT_LISTMODEL(PointMapVM, inputsList)

    // List of VM of outputs
    I2_QOBJECT_LISTMODEL(PointMapVM, outputsList)

    // Geometry for the connector in the view
    //TODO ESTIA
    // The center of the box (POINT HG)
    I2_QML_PROPERTY(QPointF, position)

    // Width of the box
    I2_QML_PROPERTY(int, width)

    // Height of the box
    I2_QML_PROPERTY(int, height)


    // Flag indicating if our agent is ON (vs OFF)
    I2_QML_PROPERTY_READONLY(bool, isON)

    // Flag indicating if our agent is reduced
    I2_QML_PROPERTY(bool, isReduced)

    // Define the value type of the reduced map (= brin) in input of the agent
    I2_QML_PROPERTY_READONLY(AgentIOPValueTypes::Value, reducedMapValueTypeInInput)

    // Define the value type of the reduced map (= brin) in output of the agent
    I2_QML_PROPERTY_READONLY(AgentIOPValueTypes::Value, reducedMapValueTypeInOutput)


public:
    /**
         * @brief Default constructor
         * @param definitionModel The first definition model needed to instanciate an agent mapping VM.
         * Typically passing during the drag-drop from the list of agent on the left side.
         * @param position Position of the box
         * @param parent
         */
    explicit AgentInMappingVM(DefinitionM * definitionModel,
                              QPointF position,
                              QObject* parent = nullptr);

    /**
         * @brief Second constructor to instanciate from a list of definition
         * @param definitionModelList The definition model list needed to instanciate an agent mapping VM.
         * Typically passing during the drag-drop from the list of agent on the left side.
         * @param position Position of the box
         * @param parent
         */
    explicit AgentInMappingVM(QList<DefinitionM *> definitionModelList,
                              QPointF position,
                              QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~AgentInMappingVM();

Q_SIGNALS:

    /**
     * @brief Signal emitted when a definition is added to the agent mapping
     * @param agentName
     */
    void newDefinitionInAgentMapping(QString agentName);

public Q_SLOTS:
    /**
         * @brief Add definition dynamically to the internal list
         * @param newDefinition The definition to add
         */
    void addDefinitionInInternalList(DefinitionM * newDefinition);

    /**
         * @brief Return the corresponding PointMap from the input IOP name
         * @param inputName
         */
    PointMapVM * getPointMapFromInputName(QString inputName);

    /**
         * @brief Return the corresponding PointMap from the output IOP name
         * @param outputName
         */
    PointMapVM * getPointMapFromOutputName(QString outputName);

private:
    // Previous list of input
    QHash<QString,PointMapVM*> _mapOfInputsFromInputName;

    // Previous list of output
    QHash<QString,PointMapVM*> _mapOfOutputsFromOutputName;

    //
    // Internal functions to factorize code
    //

    /**
         * @brief Add new points Map to the inputs list from a definition model
         * @param newDefinition The definition model
         */
        void addPointMapInInternalInputList(DefinitionM *newDefinition);

     /**
        * @brief Add new points Map to the outputs list from a definition model
        * @param newDefinition The definition model
        */
       void addPointMapInInternalOutputList(DefinitionM *newDefinition);

    /**
         * @brief This function check if the point map already exist in the list (input/output list)
         * @param list The list where to add the points map (input/output list)
         * @param agentName The name of the agent mapping VM
         * @param iopName The name of the input/output to add
         */
    bool checkIfAlreadyInList(QList<PointMapVM *> list,
                              QString agentName,
                              QString iopName);
};

QML_DECLARE_TYPE(AgentInMappingVM)

#endif // AGENTINMAPPINGVM_H
