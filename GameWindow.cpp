#include "GameWindow.h"

// ============================================================================

GameWindow::GameWindow(QWidget* parent) : QWidget(parent),
                                          sizeOfViewPort(400, 400)
{
    pause = false;
    won = false;
    gameOver = false;

    slotNewGame();

    // Оставляем активными в окне только кнопки минимизации и закрытия.
    setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    setWindowTitle("Snake"); // Задаём название окна.
}

// ----------------------------------------------------------------------------

void GameWindow::slotNewGame()
{
    // Выделяем память под игровое поле, передаём размеры для игрового поля.
    playingField = new PlayingField(sizeOfViewPort);
    scoreLabel = new QLabel; // Выделяем память под индикатор счёта.

    // Задаём фон индикатору счёта.
    scoreLabel->setStyleSheet("background: white");
    slotDisplayScore(); // Отображаем текущий счёт (0).

    // Компануем всё это.
    layout = new QVBoxLayout;

    layout->addWidget(scoreLabel);
    layout->addWidget(playingField);

    layout->setMargin(0); // Убираем отступы от края.
    layout->setSpacing(0); // Убираем свободное пространство между виджетами.

    setLayout(layout);

    // Соединяем сигналы отображения счёта, PAUSE и GAME OVER.
    connect(playingField, SIGNAL(scoreChanged()), SLOT(slotDisplayScore()));
    connect(playingField, SIGNAL(showPause()), SLOT(slotShowPause()));
    connect(playingField, SIGNAL(showGameOver()), SLOT(slotShowGameOver()));
    connect(playingField, SIGNAL(showWon()), SLOT(slotShowWon()));
}


// ----------------------------------------------------------------------------

void GameWindow::slotDisplayScore()
{
    // Получаем значение счёта.
    int value = playingField->getSnake()->getScore();

    // Задаём толщину шрифта и размещаем по центру счёт.
    QString score = "<h2><CENTER>" + QString::number(value) + "</CENTER></h2>";
    scoreLabel->setText(score); // Отображаем счёт.
}

// ----------------------------------------------------------------------------

void GameWindow::slotShowPause()
{
    // Тоже самое с паузой.
    scoreLabel->setText("<h2><CENTER>Pause!</CENTER></h2>");
}

// ----------------------------------------------------------------------------

void GameWindow::slotShowWon()
{
    won = true; // Отмечаем победу.

    // Задаём толщину шрифта, распологаем по центру и меняем цвет.
    QString textWin = "<h2><CENTER>YOU WIN!</CENTER></h2>";

    // Добавляем тэг, закрашивающий текст в зелёный цвет.
    textWin.push_front("<span style=\"color:green;\">");
    textWin.push_back("</span>");

    scoreLabel->setText(textWin); // Отображаем текст.
}

// ----------------------------------------------------------------------------

void GameWindow::slotShowGameOver()
{
    gameOver = true; // Отмечаем конец игры.

    // Задаём толщину шрифта, распологаем по центру и меняем цвет.
    QString gameOver = "<h2><CENTER>GAME OVER!</CENTER></h2>";

    // Добавляем тэг, закрашивающий текст в красный цвет.
    gameOver.push_front("<span style=\"color:red;\">");
    gameOver.push_back("</span>");

    scoreLabel->setText(gameOver); // Отображаем текст.

    // Через секунду снова отображется счёт.
    QTimer::singleShot(1000, this, SLOT(slotDisplayScore()));
}

// ----------------------------------------------------------------------------

void GameWindow::keyPressEvent(QKeyEvent* pe)
{
    // Если нажата клавиша Esc и змейка жива..

    if((pe->key() == Qt::Key_Escape) && gameIsStarted)
    {
        if(!gameOver && !won)
        {
            pause = !pause; // Меняем значение переменной паузы на противоположное.

            if(pause) // Если пауза установленна..
            {
                playingField->getSnake()->stopMoving(); // Останавливаем таймер
                slotShowPause(); // Отображаем Pause!
            }
            else
            {
                slotDisplayScore(); // Сразу отображаем счёт.
                playingField->getSnake()->continueMoving(); // Продолжаем..
            }

            return;
        }
        else
        {
            QCoreApplication::quit(); // Выход.
        }
    }
    else if((gameOver || won) && (pe->key() == Qt::Key_Return))
    {
        // При нажатии на Enter после Game Over должна начаться игра заново.
        // Для этого мы 'оживляем' змейку, вызываем деструктор у объекта
        // отобржающего сообщения, у объекта поля и у компановки.
        // Затем вызываем метод, который всё воссоздаст по новой.

        gameOver = false;
        won = false;

        // Уничтожаем всё!
        layout->~QObject();
        scoreLabel->~QObject();
        playingField->~QObject();

        slotNewGame();

        return;
    }

    // Пересылаем остальные события клавиатуры объекту игрового поля,
    // если пауза не нажата.
    if(!pause && !gameOver && !won)
        QCoreApplication::sendEvent(playingField, pe);
}

// ============================================================================
