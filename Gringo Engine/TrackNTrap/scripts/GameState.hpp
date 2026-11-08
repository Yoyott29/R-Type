#ifndef GAMESTATE_HPP_
#define GAMESTATE_HPP_

struct GameState {
    bool playing = false;
    bool canMove = false;
    bool boosted = false;
    bool finished = false;
    int respawns = 0;
};

#endif /* !GAMESTATE_HPP_ */
