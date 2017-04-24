#ifndef PLAYINGFIELD_H
#define PLAYINGFIELD_H

#include "Snake.h"

class PlayingField : public QLabel
{
    Q_OBJECT

private:
    QList<QRect> fieldSquares; // Поле разбитое на квадраты.

    Snake* snake; // Указатель на объект змейки.
    QRect feed; // Объект прмоугольника-корма, который сейчас на поле.

    bool pause; // На паузе ли игра. true - да, false - нет.
    bool gameOver; //true - да, false - нет.
    bool won;

    const int LENGTH_OF_SIDE; // Длина одной стороны одного элемента змейки.

protected:
    void paintEvent(QPaintEvent*);
    void keyPressEvent(QKeyEvent* pe);

public:
    // В конструктор передаётся требуемый размер поля.
    PlayingField(QSize area, QWidget* parent = 0);

    virtual ~PlayingField();

    Snake* getSnake() const;

signals:
    void scoreChanged(); // Сигнал об изменении счёта.
    void showPause(); // Сигнал просьбы отобразить надпись "Pause!".
    void showWon(); // Сигнал просьбы отобразить надпись 'YOU WIN!'.
    void showGameOver(); // Сигнал просьбы отобразить надпись 'GAME OVER!'.

private slots:
    void slotWon();
    void slotGameOver(); // Конец игры, змейка умерла.

public slots:
    void generateFeed(); // Сгенерировать координаты для корма.
};

#endif // PLAYINGFIELD_H
