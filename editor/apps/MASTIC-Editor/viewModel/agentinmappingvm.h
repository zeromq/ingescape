#ifndef AGENTINMAPPINGVM_H
#define AGENTINMAPPINGVM_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>

#include <viewModel/iop/inputvm.h>
#include <viewModel/iop/outputvm.h>

#include <model/agentmappingm.h>
#include <model/agentm.h>

class AgentInMappingVM : public QObject
{
    Q_OBJECT

    // Name of our agent
    I2_QML_PROPERTY_READONLY(QString, agentName)

    // List of models of agentM
    I2_QOBJECT_LISTMODEL(AgentM, agentModelList)

    // List of VM of inputs
    I2_QOBJECT_LISTMODEL(InputVM, inputsList)

    // List of VM of outputs
    I2_QOBJECT_LISTMODEL(OutputVM, outputsList)

    // The position corresponds to the corner Top-Left of the box
    I2_QML_PROPERTY(QPointF, position)

    // Flag indicating if our agent is ON (vs OFF)
    I2_QML_PROPERTY_READONLY(bool, isON)

    // Flag indicating if our agent is reduced
    I2_QML_PROPERTY(bool, isReduced)

    // Define the value type of the reduced map (= brin) in input of the agent
    I2_QML_PROPERTY_READONLY(AgentIOPValueTypes::Value, reducedMapValueTypeInInput)

    // Define the value type of the reduced map (= brin) in output of the agent
    I2_QML_PROPERTY_READONLY(AgentIOPValueTypes::Value, reducedMapValueTypeInOutput)

    // Flag indicating if our agent is a ghost agent
    I2_QML_PROPERTY_READONLY(bool, isGhost)

    // Flag indicating if all definitions are strictly identicals
    I2_QML_PROPERTY_READONLY(bool, areIdenticalsAllDefinitions)


public:
    /**
         * @brief Default constructor
         * @param agentModelList The first agentM is needed to instanciate an agent mapping VM.
         * Typically passing during the drag-drop from the list of agent on the left side.
         * @param position Position of the box
         * @param parent
         */
    explicit AgentInMappingVM(QList<AgentM*> agentModelList,
                              QPointF position,
                              QObject* parent = nullptr);

    /**
         * @brief Ghost Constructor: Definition model is empty. The agent is an empty shell only defined by a name.
         * @param agentName
         * @param parent
         */
    explicit AgentInMappingVM(QString agentName,
                              QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~AgentInMappingVM();

Q_SIGNALS:

    /**
     * @brief Signal emitted when a definition is added to the agent mapping
     * @param agentInMapping
     */
    void newDefinitionInAgentMapping(AgentInMappingVM* agentInMapping);

public Q_SLOTS:
    /**
         * @brief Add agent dynamically to the internal list
         * @param newAgent The definition to add
         */
    void addAgentToInternalList(AgentM* newAgentM);

    /**
         * @brief Return the corresponding PointMap from the input IOP name
         * @param inputName
         */
    InputVM * getPointMapFromInputName(QString inputName);

    /**
         * @brief Return the corresponding PointMap from the output IOP name
         * @param outputName
         */
    OutputVM * getPointMapFromOutputName(QString outputName);

private:
    // Previous list of input
    QHash<QString,InputVM*> _mapOfInputsFromInputName;

    // Previous list of output
    QHash<QString,OutputVM*> _mapOfOutputsFromOutputName;

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
            * @brief This function check if the OutputVM already exist in the input list
            * @param currentOuput The newly created OutputVM
            */
       bool checkIfAlreadyInOutputList(OutputVM* currentOuput);

       /**
            * @brief This function check if the InputVM already exist in the input list
            * @param currentInput The newly created Input VM
            */
       bool checkIfAlreadyInInputList(InputVM* currentInput);
};

QML_DECLARE_TYPE(AgentInMappingVM)

#endif // AGENTINMAPPINGVM_H
