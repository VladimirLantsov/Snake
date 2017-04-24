#include "GameWindow.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    GameWindow game;
    game.show();

    game.setFixedSize(game.size());

    return app.exec();
}
