#include "PlayingField.h"
#include <algorithm>
#include <stdlib.h>
#include <time.h>

// ============================================================================

PlayingField::PlayingField(QSize area, QWidget* parent) : QLabel(parent),
                                                          LENGTH_OF_SIDE(10)
{
    srand(time(NULL));

    setFixedSize(area); // Установка неизменного размера игрового поля.

    // Пауза и смерть змейки не актуальны изначально.

    pause = false;
    gameOver = false;
    won = false;

    // Разбиваем поле на квадраты, что-бы удобно работать с полем
    // и не ошибаться с координатами.

    for(int i = 0; i < area.height(); i += LENGTH_OF_SIDE)
    {
        for(int j = 0; j < area.width(); j += LENGTH_OF_SIDE)
        {
            QRect square(j, i, LENGTH_OF_SIDE, LENGTH_OF_SIDE);
            fieldSquares << square;
        }
    }

    // Вычисляем номер центрального квадрата в списке.

    int center = (fieldSquares.length() / 2) + (width() / LENGTH_OF_SIDE / 2);

    // Получаем тот квадрат, в котором будет создан первый элемент змейки
    // отностительно которого будут создаваться остальные элементы.

    QRect firstElementOfSnake = fieldSquares.at(center);

    // Создаём указатель на объект змейки.
    // Как параметры передаём полученый выше кончик хвоста,
    // размер всего поля и указатель на этот объект,
    // что бы змейка без поля не существовала.

    snake = new Snake(firstElementOfSnake, size(), this);

    // Соединяем сигнал на обновление поля, на конец игры и на просьбу о еде
    // с соответствующими слотами.

    connect(snake, SIGNAL(signalToRepaint()), SLOT(repaint()));
    connect(snake, SIGNAL(iWin()), SLOT(slotWon()));
    connect(snake, SIGNAL(snakeIsDead()), SLOT(slotGameOver()));
    connect(snake, SIGNAL(feedEnded()), SLOT(generateFeed()));

    // Пересылка сигнала об изменении счёта.

    connect(snake, SIGNAL(scoreChanged()), SIGNAL(scoreChanged()));
}

// ----------------------------------------------------------------------------

PlayingField::~PlayingField()
{

}

// ----------------------------------------------------------------------------

Snake* PlayingField::getSnake() const
{
    return snake;
}

// ----------------------------------------------------------------------------

void PlayingField::slotWon()
{
    won = true;
    snake->stopMoving();

    emit showWon();
}

// ----------------------------------------------------------------------------

void PlayingField::slotGameOver()
{
    gameOver = true; // Отмечаем смерть змейки.
    snake->stopMoving(); // Останавливаем автоматическое движение змейки.

    emit showGameOver(); // Просим отобразить GAME OVER!
}

// ----------------------------------------------------------------------------

void PlayingField::generateFeed()
{
    // Создаём список состоящий из всех свободных клеток игрового поля.
    // Для этого создаём копию игрового поля, затем обходим циклом весь список
    // элементов змейки и удаляем из списка свободных клеток совпадения.

    QList<QRect> freeCells = fieldSquares; // Здесь будут свободные клетки.
    QList<QRect> snakeElements = snake->getSnakeElements(); // Координаты змеи.

    foreach(QRect element, snakeElements)
    {
        freeCells.removeOne(element); // Удаляем совпадение координат.
    }

    // Изменяем последовательность свободных клеток для того, что бы рандом
    // был более удачным.
    std::random_shuffle(freeCells.begin(), freeCells.end());

    // Выбираем клетку для корма.
    feed = freeCells.at(rand() % (freeCells.length() - 1));

    repaint(); // Перерисовываем.

    snake->setFeed(feed); // Отправляем еду змейке.
}

// ----------------------------------------------------------------------------

void PlayingField::paintEvent(QPaintEvent*)
{
    // Создаём объект рисования и задаём цвета змейки.
    QPainter painter(this);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::red);

    // Получаем список элементов змейки.
    QList<QRect> snakeElements = snake->getSnakeElements();

    // Перебираем список и отрисовываем змейку.
    foreach(QRect element, snakeElements)
    {
        painter.drawRect(element);
    }

    // Задаём цвет корма и отображаем корм.
    painter.setBrush(Qt::green);
    painter.drawRect(feed);

    painter.end();
}

// ----------------------------------------------------------------------------

void PlayingField::keyPressEvent(QKeyEvent* pe)
{
    snake->turnSnake(pe->key()); // Передаём клавишу в метод поворота.
    snake->continueMoving(); // Возобновляем таймер.
}

// ============================================================================
