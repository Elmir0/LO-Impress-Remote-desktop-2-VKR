#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <stdexcept>
#include <QClipboard>
#include <QTimer>
#include <QKeyEvent>
#include <QShortcut>
//gggg
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_communicationService(nullptr)
    , m_commands(nullptr),
    m_errorMessageCount(0)//инициализирую
    , m_slideShow(nullptr)
{
    ui->setupUi(this);
    QShortcut *right = new QShortcut(QKeySequence(Qt::Key_Right), this);
    connect(right, &QShortcut::activated, ui->nextSlideButton, &QPushButton::animateClick);

    // Клавиша Влево (замените pushButton_Left на имя вашей кнопки)
    QShortcut *left = new QShortcut(QKeySequence(Qt::Key_Left), this);
    connect(left, &QShortcut::activated, ui->prevSlideButton_2, &QPushButton::animateClick);

    qDebug() << "=== ИНИЦИАЛИЗАЦИЯ MAINWINDOW ===";

    // Проверка кнопок
    qDebug() << "Проверка кнопок:";
    qDebug() << "  Кнопка 'Назад' (prevSlideButton_2):" << (ui->prevSlideButton_2 ? "СУЩЕСТВУЕТ" : "НЕ СУЩЕСТВУЕТ");
    qDebug() << "  Кнопка 'Вперед' (nextSlideButton):" << (ui->nextSlideButton ? "СУЩЕСТВУЕТ" : "НЕ СУЩЕСТВУЕТ");
    qDebug() << "  Кнопка 'Подключить':" << (ui->connectButton ? "СУЩЕСТВУЕТ" : "НЕ СУЩЕСТВУЕТ");
    qDebug() << "=================================";

    // Инициализация CommunicationService
    m_communicationService = new CommunicationService(this);
     m_slideShow = new slideshow(this);

    //  ПОДКЛЮЧАЕМ СИГНАЛЫ ДЛЯ PIN КОДА
    connect(m_communicationService, &CommunicationService::pairingValidationRequired,
            this, &MainWindow::onPinValidationRequired);
    connect(m_communicationService, &CommunicationService::pairingSuccessful,
            this, &MainWindow::onPairingSuccessful);
    connect(m_communicationService, &CommunicationService::connectionFailed,
            this, [this]() {
                if (ui->connectionStatusLabel->text().contains("PIN")) {
                    qDebug() << " Разрыв соединения — ожидается при PIN";
                    return;
                }

                if (m_errorMessageCount < 2) {
                    m_errorMessageCount++;
                    QMessageBox::critical(this, "Ошибка",
                                          "Не удалось подключиться к серверу");
                }
            });
    connect(m_communicationService, &CommunicationService::slideShowStarted,
            this, &MainWindow::onSlideShowStarted);
    connect(m_communicationService, &CommunicationService::slideChanged,
            this, &MainWindow::onSlideChanged);
    connect(m_communicationService, &CommunicationService::slidePreviewReceived,
            this, &MainWindow::onSlidePreviewReceived);
    connect(m_communicationService, &CommunicationService::slideNotesReceived,
            this, &MainWindow::onSlideNotesReceived);


    updateUIState(false);
}

MainWindow::~MainWindow()
{
    delete ui;
    if (m_communicationService) {
        delete m_communicationService;
    }
}

void MainWindow::setupConnections()
{
    qDebug() << "=== НАСТРОЙКА ПОДКЛЮЧЕНИЙ СИГНАЛОВ ===";

    // ПОДКЛЮЧАЕМ КНОПКИ К КОМАНДАМ:
    bool connect1 = connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::on_connectButton_clicked);
    bool connect2 = connect(ui->disconnectButton, &QPushButton::clicked, this, &MainWindow::on_disconnectButton_clicked);
    bool connect3 = connect(ui->prevSlideButton_2, &QPushButton::clicked, this, &MainWindow::on_prevSlideButton_2_clicked);
    bool connect4 = connect(ui->nextSlideButton, &QPushButton::clicked, this, &MainWindow::on_nextSlideButton_clicked);
    bool connect5 = connect(ui->startPresentationButton, &QPushButton::clicked, this, &MainWindow::on_startPresentationButton_clicked);
    bool connect6 = connect(ui->stopPresentationButton, &QPushButton::clicked, this, &MainWindow::on_stopPresentationButton_clicked);
    bool connect7 = connect(ui->blankScreenButton, &QPushButton::clicked, this, &MainWindow::on_blankScreenButton_clicked);

    qDebug() << "Подключение сигналов:";
    qDebug() << "  Подключить:" << (connect1 ? "УСПЕХ" : "ОШИБКА");
    qDebug() << "  Отключить:" << (connect2 ? "УСПЕХ" : "ОШИБКА");
    qDebug() << "  Назад:" << (connect3 ? "УСПЕХ" : "ОШИБКА");
    qDebug() << "  Вперед:" << (connect4 ? "УСПЕХ" : "ОШИБКА");
    qDebug() << "  Старт презентации:" << (connect5 ? "УСПЕХ" : "ОШИБКА");
    qDebug() << "  Стоп презентации:" << (connect6 ? "УСПЕХ" : "ОШИБКА");
    qDebug() << "  Пустой экран:" << (connect7 ? "УСПЕХ" : "ОШИБКА");
    qDebug() << "=====================================";
}

//  МЕТОД ПОДКЛЮЧЕНИЯ (С PIN КОДОМ)
void MainWindow::on_connectButton_clicked()
{
    qDebug() << "\n=== НАЖАТА КНОПКА 'ПОДКЛЮЧИТЬ' ===";

    QString ipAddress = ui->ipAddressInput->text().trimmed();
    if (ipAddress.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите IP адрес компьютера");
        return;
    }

    try {
        qDebug() << " Попытка подключения к IP:" << ipAddress;

        // Создаем сервер
        Server* server = Server::newTcpInstance(ipAddress, "Компьютер с LibreOffice");

        if (!server) {
            throw std::runtime_error("Не удалось создать сервер");
        }

        //  ИСПОЛЬЗУЕМ CommunicationService ДЛЯ ПОДКЛЮЧЕНИЯ
        m_communicationService->connectServer(server);

        qDebug() << " Подключение инициировано, ожидаю PIN код...";
        ui->currentSlideLabel->setText("Подключаемся... ожидайте PIN код");
        ui->connectionStatusLabel->setText("Статус: Ожидание аутентификации");

    } catch (const std::exception& e) {
        qDebug() << " ОШИБКА:" << e.what();
        if(m_errorMessageCount<2){
            m_errorMessageCount++;
            QMessageBox::critical(this, "Ошибка подключения",
                                  QString("Не удалось подключиться:\n%1").arg(e.what()));
        }else{
            qDebug() << "Превышен лимит сообщений об ошибках (2)";
            ui->currentSlideLabel->setText("Ошибка подключения (см. логи)");

        }
        ui->connectionStatusLabel->setText("Статус: Ошибка подключения");
        updateUIState(false);
    }
}

//  МЕТОД ДЛЯ ОБРАБОТКИ PIN КОДА
void MainWindow::onPinValidationRequired(const QString& pin)
{
    qDebug() << "\n=== ПОЛУЧЕН PIN ===";
    qDebug() << "PIN:" << pin;

    QMessageBox::information(this,
                             "Подтверждение подключения",
                             QString("Устройство: VKR Desktop\n\nPIN код: %1\n\nВведите этот код в LibreOffice Impress")
                                 .arg(pin));

    ui->currentSlideLabel->setText("Ожидаю подтверждения...");
    ui->connectionStatusLabel->setText("Статус: Введите PIN в LibreOffice");
}
//  МЕТОД ДЛЯ УСПЕШНОЙ АУТЕНТИФИКАЦИИ
void MainWindow::onPairingSuccessful()
{
    m_errorMessageCount = 0;

    m_commands = m_communicationService->getCommandsTransmitter();
    if (!m_commands) {
        qDebug() << "CommandsTransmitter == nullptr";
        return;
    }

    updateUIState(true);

    ui->currentSlideLabel->setText("Подключено!");
    ui->connectionStatusLabel->setText("Статус: Подключено");

    QMessageBox::information(this, "Успех",
                             " Подключение установлено!\n\n"
                             "Теперь вы можете управлять презентацией:\n"
                             "• Кнопки 'Вперед' и 'Назад' для переключения слайдов\n"
                             "• 'Старт презентации' для начала показа\n"
                             "• 'Пустой экран' для паузы");
}
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Right) {
        // Действие для стрелки Вправо
        ui->nextSlideButton->animateClick();
    }
    else if (event->key() == Qt::Key_Left) {
        // Действие для стрелки Влево
        ui->prevSlideButton_2->animateClick();
    }
    else {
        QMainWindow::keyPressEvent(event);
    }
}

//  МЕТОД ОТКЛЮЧЕНИЯ
void MainWindow::on_disconnectButton_clicked()
{
    qDebug() << "\n=== НАЖАТА КНОПКА 'ОТКЛЮЧИТЬ' ===";

    // СБРАСЫВАЕМ СТАТИЧЕСКИЙ ФЛАГ
    {
        static bool reset = false;
        if (!reset) {
            // Вызываем метод для сброса статического флага
            // (или используем другой механизм)
            reset = true;
        }
    }

    if (m_communicationService) {
        try {
            m_communicationService->disconnectServer();
            updateUIState(false);
            ui->currentSlideLabel->setText("Не подключено");
            ui->connectionStatusLabel->setText("Статус: Отключено");
            qDebug() << " Отключено успешно";

            m_commands = nullptr;
            m_errorMessageCount = 0;
        } catch (const std::exception& e) {
            qDebug() << "ОШИБКА ПРИ ОТКЛЮЧЕНИИ:" << e.what();
            QMessageBox::critical(this, "Ошибка",
                                  QString("Не удалось отключиться:\n%1").arg(e.what()));
        }
    }
}
//  КНОПКА "НАЗАД"
void MainWindow::on_prevSlideButton_2_clicked()
{
    if (m_communicationService) {
        ui->prevSlideButton_2->setEnabled(false);
        m_communicationService->previousSlide();
        QTimer::singleShot(500, this, [this]() {
            if (m_commands) ui->prevSlideButton_2->setEnabled(true);
        });
    }
}

//  КНОПКА "ВПЕРЕД"
void MainWindow::on_nextSlideButton_clicked()
{
    if (m_communicationService) {
        ui->nextSlideButton->setEnabled(false);
        m_communicationService->nextSlide();
        QTimer::singleShot(500, this, [this]() {
            if (m_commands) ui->nextSlideButton->setEnabled(true);
        });
    }
}

//  КНОПКА "СТАРТ ПРЕЗЕНТАЦИИ"
void MainWindow::on_startPresentationButton_clicked()
{
    qDebug() << "\n=== НАЖАТА КНОПКА 'СТАРТ ПРЕЗЕНТАЦИИ' ===";

    if (m_commands) {
        m_commands->startPresentation();
    } else {
        QMessageBox::warning(this, "Ошибка", "Сначала подключитесь к компьютеру!");
    }
}

//  КНОПКА "СТОП ПРЕЗЕНТАЦИИ"
void MainWindow::on_stopPresentationButton_clicked()
{
    qDebug() << "\n=== НАЖАТА КНОПКА 'СТОП ПРЕЗЕНТАЦИИ' ===";

    if (m_commands) {
        m_commands->stopPresentation();
    } else {
        QMessageBox::warning(this, "Ошибка", "Сначала подключитесь к компьютеру!");
    }
}

//  КНОПКА "ПУСТОЙ ЭКРАН"
void MainWindow::on_blankScreenButton_clicked()
{
    qDebug() << "\n=== НАЖАТА КНОПКА 'ПУСТОЙ ЭКРАН' ===";

    if (m_commands) {
        m_commands->setUpBlankScreen();
    } else {
        QMessageBox::warning(this, "Ошибка", "Сначала подключитесь к компьютеру!");
    }
}

//  ОБНОВЛЕНИЕ ИНТЕРФЕЙСА
void MainWindow::updateUIState(bool connected)
{
    qDebug() << "\n=== ОБНОВЛЕНИЕ СОСТОЯНИЯ ИНТЕРФЕЙСА ===";
    qDebug() << "Новое состояние:" << (connected ? "ПОДКЛЮЧЕНО" : "ОТКЛЮЧЕНО");

    // Кнопки управления включаем только если есть CommandsTransmitter
    bool canControl = connected && m_commands != nullptr;

    ui->connectButton->setEnabled(!connected);
    ui->disconnectButton->setEnabled(connected);
    ui->ipAddressInput->setEnabled(!connected);

    ui->prevSlideButton_2->setEnabled(canControl);
    ui->nextSlideButton->setEnabled(canControl);
    ui->startPresentationButton->setEnabled(canControl);
    ui->stopPresentationButton->setEnabled(canControl);
    ui->blankScreenButton->setEnabled(canControl);

    qDebug() << "Кнопка 'Назад':" << (ui->prevSlideButton_2->isEnabled() ? "ВКЛ" : "ВЫКЛ");
    qDebug() << "Кнопка 'Вперед':" << (ui->nextSlideButton->isEnabled() ? "ВКЛ" : "ВЫКЛ");
    qDebug() << "======================================\n";
}

// Пустые методы (можно реализовать позже)
void MainWindow::onSocketConnected() {}
void MainWindow::onSocketDisconnected() {}
void MainWindow::onSocketError(const QString& error) {
    qDebug() << "Ошибка сокета:" << error;
}

void MainWindow::onSlideShowStarted(int slidesCount, int currentSlide)
{
    m_slideShow->setSlidesCount(slidesCount);
    m_slideShow->setCurrentSlideIndex(currentSlide);
    m_slideShow->setRunning(true);
    ui->currentSlideLabel->setText(
        QString("Слайд %1 / %2").arg(currentSlide + 1).arg(slidesCount));
}

void MainWindow::onSlideChanged(int currentSlide)
{
    m_slideShow->setCurrentSlideIndex(currentSlide);

    ui->currentSlideLabel->setText(
        QString("Слайд %1 / %2")
            .arg(currentSlide + 1)
            .arg(m_slideShow->getSlidesCount()));

    // Показываем заметки
    QString notes = m_slideShow->getSlideNotes(currentSlide);
    if (notes.isEmpty() || notes == "<html><body><br/></body></html>") {
        ui->notesContentLabel->setText("Нет заметок");
    } else {
        QString plainNotes = notes;
        plainNotes.remove(QRegularExpression("<[^>]*>"));
        plainNotes = plainNotes.trimmed();
        ui->notesContentLabel->setText(plainNotes.isEmpty() ? "Нет заметок" : plainNotes);
    }

    updateSlideDisplay(currentSlide);
    updateNextSlidesList(currentSlide);
}

void MainWindow::onSlidePreviewReceived(int slideIndex, const QByteArray& preview)
{
    m_slideShow->setSlidePreviewBytes(slideIndex, preview);

    // Если это текущий слайд — сразу показываем
    if (slideIndex == m_slideShow->getCurrentSlideIndex()) {
        updateSlideDisplay(slideIndex);
    }
    // Обновляем список следующих слайдов
    updateNextSlidesList(m_slideShow->getCurrentSlideIndex());
}

void MainWindow::onSlideNotesReceived(int slideIndex, const QString& notes)
{
    m_slideShow->setSlideNotes(slideIndex, notes);

    // Если это текущий слайд - сразу показываем
    if (slideIndex == m_slideShow->getCurrentSlideIndex()) {
        if (notes.isEmpty() || notes == "<html><body><br/></body></html>") {
            ui->notesContentLabel->setText("Нет заметок");
        } else {
            QString plainNotes = notes;
            plainNotes.remove(QRegularExpression("<[^>]*>"));
            plainNotes = plainNotes.trimmed();
            ui->notesContentLabel->setText(plainNotes.isEmpty() ? "Нет заметок" : plainNotes);
        }
    }
}

void MainWindow::updateSlideDisplay(int slideIndex)
{
    QByteArray previewBytes = m_slideShow->getSlidePreviewBytes(slideIndex);
    if (previewBytes.isEmpty()) {
        ui->mainSlideLabel->setText("Загрузка слайда...");
        return;
    }

    QPixmap pixmap;
    if (pixmap.loadFromData(previewBytes)) {
        // Масштабируем под размер label с сохранением пропорций
        QPixmap scaled = pixmap.scaled(
            ui->mainSlideLabel->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            );
        ui->mainSlideLabel->setPixmap(scaled);
        ui->mainSlideLabel->setText(""); // убираем текст
    } else {
        ui->mainSlideLabel->setText("Ошибка загрузки слайда");
    }
}

void MainWindow::updateNextSlidesList(int currentSlide)
{
    ui->nextSlidesList->clear();

    int totalSlides = m_slideShow->getSlidesCount();
    // Показываем следующие 5 слайдов
    for (int i = currentSlide + 1; i < qMin(currentSlide + 6, totalSlides); ++i) {
        QListWidgetItem* item = new QListWidgetItem();
        item->setText(QString("Слайд %1").arg(i + 1));

        QByteArray previewBytes = m_slideShow->getSlidePreviewBytes(i);
        if (!previewBytes.isEmpty()) {
            QPixmap pixmap;
            if (pixmap.loadFromData(previewBytes)) {
                QPixmap scaled = pixmap.scaled(200, 120,
                                               Qt::KeepAspectRatio, Qt::SmoothTransformation);
                item->setIcon(QIcon(scaled));
            }
        }
        ui->nextSlidesList->addItem(item);
    }

    // размер иконок
    ui->nextSlidesList->setIconSize(QSize(200, 120));
}

