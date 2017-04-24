#include "Snake.h"

// ============================================================================

Snake::Snake(QRect firstElement, QSize fieldSize, QObject* parent)
                       : QObject(parent), LENGTH_OF_SIDE(firstElement.width())
{
    // Сохранение размера поля в котором создаётся змейка.
    playingFieldSz = fieldSize;

    // Стартовое положение змейки.
    currentTurn = RIGHT;

    // Стартовое количество элементов змейки.
    const int START_LENGTH = 4;

    // Помещаем в список элементов змейки первый элемент.
    elementsOfSnake.append(firstElement);

    // Получаем координаты первого элемента.
    int x = firstElement.x();
    int y = firstElement.y();

    // Создаём и помещаем все остальные элементы в список.
    for(int i = 1; i < START_LENGTH; ++i)
    {
        QRect element(x + i * LENGTH_OF_SIDE, y, LENGTH_OF_SIDE,
                                                          LENGTH_OF_SIDE);
        elementsOfSnake.append(element);
    }

    won = false;
    dead = false;

    // Начальный счёт и начальный период движения змейки вперёд.
    score = 0;
    speed = 100;

    //Выделяем память под таймер, сделав объект змеи его родительским объектом.
    timeToMove = new QTimer(this);

    // Соединяем сигнал окончания цикла таймера со слотом передвижения змейки
    // вперёд и запускаем таймер на заданный интервал.
    connect(timeToMove, SIGNAL(timeout()), SLOT(moveSnakeForward()));
    timeToMove->start(speed);
}

// ----------------------------------------------------------------------------

int Snake::getScore() const
{
    return score;
}

// ----------------------------------------------------------------------------

bool Snake::isDead() const
{
    return dead;
}

// ----------------------------------------------------------------------------

QList<QRect>Snake::getSnakeElements()
{
    return elementsOfSnake;
}

// ----------------------------------------------------------------------------

void Snake::stopMoving()
{
    if(timeToMove->isActive()) // Если таймер активирован сейчас..
        timeToMove->stop(); // Останавливаем.
}

// ----------------------------------------------------------------------------

void Snake::continueMoving()
{
    // Если таймер не активен и змейка жива.
    if(!timeToMove->isActive() && !dead && !won)
        timeToMove->start(speed); // Запускам таймер с той-же скоростью.
}

// ----------------------------------------------------------------------------

void Snake::_checkBorder()
{
    QPoint headCoordinates = elementsOfSnake.last().topLeft(); // Кооридинаты
                                                               // головы.

    if(headCoordinates.x() >= playingFieldSz.width()) // Если голова прошла
    {                                                 // через правую стенку.
        headCoordinates.rx() = 0;
    }
    else if(headCoordinates.x() < 0) // Если голова прошла через левую стенку.
    {
        headCoordinates.rx() = playingFieldSz.width();
    }
    else if(headCoordinates.y() < 0) // Если голова прошла через верхнюю стенку
    {
        headCoordinates.ry() = playingFieldSz.height();
    }
    else if(headCoordinates.y() >= playingFieldSz.height()) // Если голова
    {                                            // прошла через нижнюю стенку.
        headCoordinates.ry() = 0;
    }

    // Если прошлые координаты головы не равны новым,
    // то перемещаем голову за стенку.

    if((elementsOfSnake.last().topLeft()) != headCoordinates)
    {
        // Перемещаем голову на новые координаты.
        elementsOfSnake.last().moveTo(headCoordinates);

        _checkCollision(); // Проверка на столкновения головы с телом.
        _checkFeed(); // А не попали ли мы на корм....
    }
}

// ----------------------------------------------------------------------------

void Snake::_checkCollision()
{
    for(int i = 0; i < elementsOfSnake.length() - 1; ++i) // Обходим все
    {                                                    // элементы змейки...

        // Если голова змейки находится там-же где и один из элементов змейки.
        if(elementsOfSnake.at(i) == elementsOfSnake.last())
        {
            dead = true; // Да, теперь змейка мертва. Отмечаем это...
            emit snakeIsDead(); // и оповещаем об этом тех кому это надо..
        }
    }
}

// ----------------------------------------------------------------------------

void Snake::_checkFeed()
{
    if(feedForSnake.isNull()) // Если еда для змейки имеется на поле(на всякий)
    {
        emit feedEnded(); // Требуем корм!
        return;
    }

    QRect head = elementsOfSnake.last(); // Получаем голову змейки.

    if(feedForSnake == head) // Если корм нахоидтся там-же где и голова.
    {
        emit feedEnded(); // Запрашиваем генерацию нового корма.

        _changeProgress(); // Увеличиваем счёт и если надо скорость.

        _addPointToStartIncreasing(); // Сохраняем точку, в которой
                                      // будет увеличена змейка.
    }
}

// ----------------------------------------------------------------------------

void Snake::_checkPointToStartIncreasing()
{
    if(!pointToStartIncreasing.isEmpty()) // Если список точек
    {                                     // для увеличения не пуст..

        QPoint tail = elementsOfSnake.first().topLeft(); // Получаем координату
                                                         //  кончика хвоста.

        foreach(QPoint point, pointToStartIncreasing) // Перебираем точки
        {                                             // увеличения..
            if(tail == point) // Если кончик хвоста совпадает
            {                 // с точкой увеличения..

                pointToStartIncreasing.removeOne(point); // Удаляем совпавшую
                                                         // точку из списка..
                _tailLonger(); // Увеличить длину хвоста.
            }
        }
    }
}

// ----------------------------------------------------------------------------

void Snake::moveSnakeForward()
{
    if(!elementsOfSnake.isEmpty()) // Если список элементов не пуст.
    {
         // Текущая позиция головы.
        QPoint lastPos = elementsOfSnake.last().topLeft();

        // Получаем координаты головы.
        int x = elementsOfSnake.last().x();
        int y = elementsOfSnake.last().y();

        // Перемещаем вперёд змейку в завимости от того куда она повёрнута.
        switch(currentTurn)
        {
        case RIGHT:
            elementsOfSnake.last().moveTo(x + LENGTH_OF_SIDE, y);
            break;

        case LEFT:
            elementsOfSnake.last().moveTo(x - LENGTH_OF_SIDE, y);
            break;

        case UP:
            elementsOfSnake.last().moveTo(x, y - LENGTH_OF_SIDE);
            break;

        case DOWN:
            elementsOfSnake.last().moveTo(x, y + LENGTH_OF_SIDE);
            break;
        }

        _checkCollision(); // Проверяем столкновения.
        _checkBorder(); // Проверяем границы.
        _checkFeed(); // Проверяем корм.
        _checkPointToStartIncreasing(); // Проверяем не нужно ли увеличить
                                        // размер змейки.

        _cathUpHead(lastPos); // Хвост догоняет голову.
                              // В параметр передаётся прошлая позиция головы.
    }
}

// ----------------------------------------------------------------------------

void Snake::_tailLonger()
{
    // Получаем координаты головы.

    int x = elementsOfSnake.last().topLeft().x();
    int y = elementsOfSnake.last().topLeft().y();

    // Изменяем координаты по текущему положению змейки.
    switch(currentTurn)
    {
    case RIGHT:
        x += LENGTH_OF_SIDE;
        break;

    case LEFT:
        x -= LENGTH_OF_SIDE;
        break;

    case UP:
        y -= LENGTH_OF_SIDE;
        break;

    case DOWN:
        y += LENGTH_OF_SIDE;
        break;
    }

    QRect element(x, y, LENGTH_OF_SIDE, LENGTH_OF_SIDE); //Создаём новый элемент
    elementsOfSnake.push_front(element); // И помещаем его в начало, т.е. в хвост.

    emit signalToRepaint(); // Перерисовываем поле.

    _checkCollision(); // Проверяем на столкновения.
    _checkBorder(); // Проверяем границы.
}

// ----------------------------------------------------------------------------

void Snake::setFeed(QRect feed)
{
    feedForSnake = feed; // Сохраняем корм.
}

// ----------------------------------------------------------------------------

void Snake::_cathUpHead(QPoint lastPos)
{
     // Перебираем змейку от элемента стоящего перед головой до хвоста
    for(int i = elementsOfSnake.length() - 2; i >= 0; --i)
    {
        // Временно сохраняем координату текущего элемента.
        QPoint temp = elementsOfSnake.at(i).topLeft();
        // перемещаем текущий элемент на кооридинату переданную в параметре.
        elementsOfSnake[i].moveTo(lastPos);

        // присваиваем к переменной из параметров
        // прошлую координату текущего элемента
        lastPos = temp;
    }

    emit signalToRepaint(); // Перерисовываем.
}

// ----------------------------------------------------------------------------

void Snake::_addPointToStartIncreasing()
{
    // Сохраняем координаты той точки где по ела змейка.
    QPoint head = elementsOfSnake.last().topLeft();
    pointToStartIncreasing << head;
}

// ----------------------------------------------------------------------------

void Snake::_changeProgress()
{
    score += 10; // Увеличиваем значение счёта на 10
    emit scoreChanged(); // и высылаем сигнал об этом.

    const int SCORE_MAXIMUM = playingFieldSz.width() * playingFieldSz.height()
                                                              / LENGTH_OF_SIDE;

    if(score == SCORE_MAXIMUM)
    {
        won = true;
        emit iWin();
    }

    // Если частота вызова метода движения вперёд не достигла 50
    // и текущий счёт является одним из сотых значений счёта - уменьшаем интервал.
    if((speed != 50) && !(score % 100))
    {
        --speed;
    }
}

// ----------------------------------------------------------------------------

void Snake::turnSnake(const int turn)
{
    QPoint pos = elementsOfSnake.last().topLeft(); // Текущая координата головы
    QPoint temp = pos; // Временно сохраняем текущую координату головы.

    if((turn == Qt::Key_Right) && (currentTurn != LEFT))
    {
        // Если нажата клавиша право и не повёрнута змейка влево.
        currentTurn = RIGHT;
        pos.rx() += LENGTH_OF_SIDE;
    }
    else if((turn == Qt::Key_Left) && (currentTurn != RIGHT))
    {
        // Если нажата клавиша лево и не повёрнута змейка вправо.
        currentTurn = LEFT;
        pos.rx() -= LENGTH_OF_SIDE;
    }
    else if((turn == Qt::Key_Up) && (currentTurn != DOWN))
    {
        // Если нажата клавиша вверх и не повёрнута змейка вниз.
        currentTurn = UP;
        pos.ry() -= LENGTH_OF_SIDE;
    }
    else if((turn == Qt::Key_Down) && (currentTurn != UP))
    {
        // Если нажата клавиша низ и не повёрнута змейка вверх.
        currentTurn = DOWN;
        pos.ry() += LENGTH_OF_SIDE;
    }
    else
    {
        // Если что-то из этого всего не вышло
        // Таймер продолжает свою работу и змейка двигается сама.
        emit continueMoving();
        return;
    }

    emit stopMoving(); // Если нажата клавиша-стрелочка,
                      // то остнаваливаем автоматическое движение змейки.

    elementsOfSnake.last().moveTo(pos); // Перемещаем голову змейки
                                       // на координату полученную выше.

    _checkCollision(); // Проверяем столкновения.
    _checkBorder(); // Проверяем границы.
    _checkFeed(); // Проверяем корм.
    _checkPointToStartIncreasing();// Проверяем достижение головы точки,
                                   // на которой можно увеличить длинну хвоста.

    _cathUpHead(temp); // Хвост догоняет голову.
}

// ============================================================================
