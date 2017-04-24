#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include "PlayingField.h"

class GameWindow : public QWidget
{
    Q_OBJECT

private:
    PlayingField* playingField; // Создаём указатель на игровое поле.
    QLabel* scoreLabel; // Указатель на индикатор счёта.

    bool gameIsStarted; // Началась ли игра.
    bool pause; // Флаг паузы
    bool won; // Флаг победы змейки.
    bool gameOver; // Флаг не удачного окончания игры

    const QSize sizeOfViewPort;

    QVBoxLayout* layout; // Компановка главного окна.

protected:
    void keyPressEvent(QKeyEvent* pe);

public:
    GameWindow(QWidget* parent = 0);

private slots:
    void slotNewGame(); // Начать новую игру.

    void slotDisplayScore(); // Слот отображения очков.
    void slotShowPause(); // Слот отображения надписи Pause
    void slotShowWon(); // Слот победы змейки.
    void slotShowGameOver(); // Слот отображения надписи GAME OVER!
};

#endif // GAMEWINDOW_H
