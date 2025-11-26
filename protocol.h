#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <QString>


class Protocol{
    Protocol()=delete;
public:
    static const QString CHARSET;

    class Ports{
    Ports() = delete;
    public:
        // порты
        static const int   SERVER_SEARCH=1598;
        static const int   CLIENT_CONNECTION=1599;
    };

    // адреса команд
    class Addresses{
    Addresses() = delete;
    public:
        static const QString SERVER_SEARCH;
    };

    // смс
    class Messages{
    Messages() = delete;
    public:
        static const QString PAIRED;
        static const QString VALIDATING;
        static const QString ADVERTISE;
        static const QString SLIDE_SHOW_STARTED;
        static const QString SLIDE_SHOW_FINISHED;
        static const QString SLIDE_UPDATED;
        static const QString SLIDE_PREVIEW;
        static const QString SLIDE_NOTES;

    };

    class Commands{
    Commands() = delete;
    public:
        static const QString PAIR_WITH_SERVER;
        static const QString SEARCH_SERVERS;

        static const QString TRANSITION_NEXT;
        static const QString TRANSITION_PREVIOUS;
        static const QString GO_TO_SLIDE;
        static const QString PRESENTATION_BLANK_SCREEN;
        static const QString PRESENTATION_RESUME;
        static const QString PRESENTATION_START;
        static const QString PRESENTATION_STOP;

        static const QString POINTER_STARTED;
        static const QString POINTER_COORDINATION;
        static const QString POINTER_DISMISSED;

        static  QString prepareCommand(const QString& command );
        static  QString prepareCommand(const QStringList&parameters);

    private:
        static const QString DELIMITER_PARAMETER;
        static const QString DELIMITER_COMMAND;

    };

    class Pin{
    private:
        static const int NUMBERS_COUNT =4;
        static int generatePinNumber();
        static QString generatePinText();
        static int preventLeadingZeros(int pin);
        static QString buildPinFormat();
    public:
        static QString generate();
        Pin()=delete;

    };

};

#endif // PROTOCOL_H
