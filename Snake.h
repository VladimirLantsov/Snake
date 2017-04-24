#ifndef SNAKE_H
#define SNAKE_H

#include <QtWidgets>

enum Turns {LEFT, RIGHT, UP, DOWN}; // Перечисление поворотов.

class Snake : public QObject
{
    Q_OBJECT

private:
    QSize playingFieldSz; // Размер поля, в котором отображается змейка.

    QTimer* timeToMove; // Таймер, для автоматического движения змейки вперёд.

    const int LENGTH_OF_SIDE; // Длина одной стороны элементы змейки.

    int score; // Счёт.
    int speed; // Временной интервал вызова слота движения вперёд.

    bool won;  // true - змейка победали. false - пока нет.
    bool dead; // true - если змейка мертва, false - жива.

    QRect feedForSnake; // Корм для змейки, на данный момент.

    QList<QRect> elementsOfSnake; // Список элементов змейки.
    QList<QPoint> pointToStartIncreasing; // Точка, в которой змейка
                                          //увеличится на 1 элемент.

    Turns currentTurn; // Текущий поворот змейки.

    void _cathUpHead(QPoint lastPos); // Догнать голову.
    void _changeProgress(); // увеличить очки и если нужно частоту движения.
    void _addPointToStartIncreasing(); // Добавить точку начала увеличения змейки.
    void _tailLonger(); // Увеличить длину хвоста змейки.

    void _checkCollision(); // Проверить на столкновение змейки с самой собой.
    void _checkBorder(); // Проверить границы поля.
    void _checkFeed(); // Проверить корм.
    void _checkPointToStartIncreasing(); // Проверить точку начала роста змейки.

public:
    // В конструктор передаётся прямоугольник, соответствующий кончику
    // хвоста, размер поля в котором будет змейка и указатель на
    // родительский объект.

    Snake(QRect firstElement, QSize sizeOfField, QObject* parent = 0);

    QList<QRect> getSnakeElements(); // Вернуть элементы змейки.

    void turnSnake(const int turn); // Повернуть змейку.
    void setFeed(QRect feed); // Задать элемент корма змейки.

    void stopMoving(); // Остановка автоматического движения змейки.
    void continueMoving(); // Разрешить автоматическое движение змейки.

    int getScore() const; // Получить текущие очки.

    bool isDead() const; // Возвращает true если змейка мертва.

signals:
    void signalToRepaint(); // Нужно перериосовать содержимое поля.
    void scoreChanged(); // Сигнал об изменении счёта.
    void feedEnded(); // Нужно сгенерировать корм для змейки.
    void iWin(); // Змейка победила!
    void snakeIsDead(); // Змейка умерла.

private slots:
    void moveSnakeForward(); // Двигаться вперёд, в зависимости от поворота.
};

#endif // SNAKE_H
