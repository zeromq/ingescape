#include "elementmappingm.h"

/**
     * @brief Constructor with input and output agent and IOP
     * @param inputAgent
     * @param input
     * @param outputAgent
     * @param output
     * @param parent
     */
ElementMappingM::ElementMappingM(QString inputAgent,
                             QString input,
                             QString outputAgent,
                               QString output,
                             QObject *parent) : QObject(parent),
    _inputAgent(inputAgent),
    _input(input),
    _outputAgent(outputAgent),
    _output(output)
{
    qInfo() << "New Model of Element Mapping has been created between Agent input: " << inputAgent << "." << input << " and Agent output: " << outputAgent << "." << output;
}


/**
 * @brief Destructor
 */
ElementMappingM::~ElementMappingM()
{
    qInfo() << "Delete Model of element mapping ...";
}
