#include "commandstransmitter.h"
CommandsTransmitter::CommandsTransmitter(TcpServerConnection*aServerConnection):mCommandsWriter(buildCommandsWriter(aServerConnection)){}

QIODevice* CommandsTransmitter::buildCommandsWriter(TcpServerConnection* aServerConnection) {
    return aServerConnection->buildCommandsStream();
}

void CommandsTransmitter::pair(const QString & aDeviceName,const QString & aPin){
    QStringList parameters;
    parameters <<Protocol::Commands::PAIR_WITH_SERVER<<aDeviceName<<aPin;
    writeCommand(Protocol::Commands::prepareCommand(parameters));
}

void CommandsTransmitter::performNextTransition(){
    writeCommand(Protocol::Commands::prepareCommand(Protocol::Commands::TRANSITION_NEXT));
}

void CommandsTransmitter::performPreviousTransition(){
    writeCommand(Protocol::Commands::prepareCommand(Protocol::Commands::TRANSITION_PREVIOUS));
}

void CommandsTransmitter::setCurrentSlide(int slideIndex) {
    QStringList parameters;
    parameters << Protocol::Commands::GO_TO_SLIDE << QString::number(slideIndex);
    writeCommand(Protocol::Commands::prepareCommand(parameters));
}

void CommandsTransmitter::setUpBlankScreen(){
    writeCommand(Protocol::Commands::prepareCommand(Protocol::Commands::PRESENTATION_BLANK_SCREEN));
}

void CommandsTransmitter::resumePresentation(){
    writeCommand(Protocol::Commands::prepareCommand(Protocol::Commands::PRESENTATION_RESUME));
}

void CommandsTransmitter::startPresentation(){
    writeCommand(Protocol::Commands::prepareCommand(Protocol::Commands::PRESENTATION_START));
}

void CommandsTransmitter::stopPresentation(){
    writeCommand(Protocol::Commands::prepareCommand(Protocol::Commands::PRESENTATION_STOP));
}

void CommandsTransmitter::startPointer(float x,float y){
    QStringList parameters;
    parameters<<Protocol::Commands::POINTER_STARTED<<QString ::number(x)<<QString::number(y);
    writeCommand(Protocol::Commands::prepareCommand(parameters));
}

void CommandsTransmitter::movePointer(float x,float y){
    QStringList parameters;
    parameters<<Protocol::Commands::POINTER_COORDINATION<<QString::number(x)<<QString::number(y);
    writeCommand(Protocol::Commands::prepareCommand(parameters));
}

void CommandsTransmitter::stopPointer(){
    writeCommand(Protocol::Commands::prepareCommand(Protocol::Commands::POINTER_DISMISSED));
}

void CommandsTransmitter::writeCommand(const QString& aCommand) {
    QByteArray data = aCommand.toUtf8();
    mCommandsWriter->write(data);

    qDebug() << "Command sent:" << aCommand.trimmed();
}


















