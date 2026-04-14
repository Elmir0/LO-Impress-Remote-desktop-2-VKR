#include "protocol.h"
#include <QRandomGenerator>
#include <QDateTime>
#include <qmath.h>
#include <QStringList>

const QString Protocol::CHARSET="UTF-8";
const QString Protocol::Addresses::SERVER_SEARCH ="239.0.0.1";

const QString Protocol::Messages::PAIRED = "LO_SERVER_SERVER_PAIRED";
const QString Protocol::Messages::VALIDATING = "LO_SERVER_VALIDATING_PIN";
const QString Protocol::Messages::ADVERTISE = "LOREMOTE_ADVERTISE";

const QString Protocol::Messages::SLIDE_SHOW_STARTED = "slideshow_started";
const QString Protocol::Messages::SLIDE_SHOW_FINISHED = "slideshow_finished";
const QString Protocol::Messages::SLIDE_UPDATED ="slide_updated";
const QString Protocol::Messages::SLIDE_PREVIEW = "slide_preview";
const QString Protocol::Messages::SLIDE_NOTES =  "slide_notes";

const QString Protocol::Commands::PAIR_WITH_SERVER = "LO_SERVER_CLIENT_PAIR";
const QString Protocol::Commands::SEARCH_SERVERS = "LOREMOTE_SEARCH";

const QString Protocol::Commands::TRANSITION_NEXT = "transition_next";
const QString Protocol::Commands::TRANSITION_PREVIOUS = "transition_previous";
const QString Protocol::Commands::GO_TO_SLIDE = "goto_slide";
const QString Protocol::Commands::PRESENTATION_BLANK_SCREEN = "presentation_blank_screen";
const QString Protocol::Commands::PRESENTATION_RESUME = "presentation_resume";
const QString Protocol::Commands::PRESENTATION_START = "presentation_start";
const QString Protocol::Commands::PRESENTATION_STOP = "presentation_stop";

const QString Protocol::Commands::POINTER_STARTED = "pointer_started";
const QString Protocol::Commands::POINTER_COORDINATION = "pointer_coordination";
const QString Protocol::Commands::POINTER_DISMISSED = "pointer_dismissed";

const QString Protocol::Commands::DELIMITER_PARAMETER = "\n";
const QString Protocol::Commands::DELIMITER_COMMAND = "\n\n";

// Методы Commands
QString Protocol::Commands::prepareCommand(const QString& command) {
    return command + DELIMITER_COMMAND;
}

QString Protocol::Commands::prepareCommand(const QStringList& parameters) {
    QString command = parameters.join(DELIMITER_PARAMETER);
    return prepareCommand(command);
}

// Методы Pin
QString Protocol::Pin::generate() {
    return generatePinText();
}

QString Protocol::Pin::generatePinText() {
    int pin = preventLeadingZeros(generatePinNumber());
    return QString("%1").arg(pin, NUMBERS_COUNT, 10, QChar('0'));
}

int Protocol::Pin::generatePinNumber() {
    int maxPin = (int)qPow(10, NUMBERS_COUNT) - 1;
    return QRandomGenerator::global()->bounded(1, maxPin + 1);
}

int Protocol::Pin::preventLeadingZeros(int pin) {
    int minPin = (int)qPow(10, NUMBERS_COUNT - 1);
    return (pin >= minPin) ? pin : pin + minPin;
}

QString Protocol::Pin::buildPinFormat() {
    return QString("%%0%1d").arg(NUMBERS_COUNT);
}
