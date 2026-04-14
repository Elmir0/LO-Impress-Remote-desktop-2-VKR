#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include "server.h"
#include "TcpServerConnection.h"
#include "commandstransmitter.h"
#include "communicationservice.h"
#include "pairingprovider.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    //  ОСНОВНЫЕ КОМАНДЫ ДЛЯ КНОПОК:
    void on_connectButton_clicked();           //  Подключение
    void on_disconnectButton_clicked();        //  Отключение
    void on_prevSlideButton_2_clicked();         // НАЗАД (главная команда)
    void on_nextSlideButton_clicked();         //  ВПЕРЕД (главная команда)
    void on_startPresentationButton_clicked(); //  Начать презентацию
    void on_stopPresentationButton_clicked();  //  Остановить презентацию
    void on_blankScreenButton_clicked();       // Пустой экран

    // Обработчики состояния подключения
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError(const QString& error);

    void onPinValidationRequired(const QString& pin);
    void onPairingSuccessful();
    void onSlideShowStarted(int slidesCount, int currentSlide);
    void onSlideChanged(int currentSlide);
    void onSlidePreviewReceived(int slideIndex, const QByteArray& preview);
    void onSlideNotesReceived(int slideIndex, const QString& notes);

    // вспомогательный метод:
    void updateSlideDisplay(int slideIndex);
    void updateNextSlidesList(int currentSlide);
private:
    Ui::MainWindow *ui;

    CommunicationService* m_communicationService;
    CommandsTransmitter* m_commands;


    void setupConnections();
    void updateUIState(bool connected);
    int m_errorMessageCount;  // Счетчик сообщений об ошибках
    QString m_currentPin;  // Сохраняем текущий PIN
    slideshow* m_slideShow;
protected:
     void keyPressEvent(QKeyEvent *event) override;

};

#endif // MAINWINDOW_H
