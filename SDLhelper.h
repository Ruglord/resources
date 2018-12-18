#ifndef SDLHELPER_H_INCLUDED
#define SDLHELPER_H_INCLUDED

#include "SDL.h"

#include "vanilla.h"


class KeyManager : public NumberManager //the keymanager stores numbers pressed in order based on the most recent key pressed.
{
    SDL_Keycode justPressed = -1;
public:
    KeyManager()
    {

    }
    double getLatest();
    SDL_Keycode getLater(double m, double n); //of m and n, finds which key which pressed later/recently.
    void addNumber(double key);
    int findNumber(double n); //finds the index of n or -1 if n isn't found
    void getKeys(SDL_Event& e);
    SDL_Keycode getJustPressed();






};



#endif // SDLHELPER_H_INCLUDED
