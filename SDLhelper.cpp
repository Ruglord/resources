#include "SDLhelper.h"

    double KeyManager::getLatest()
    {
        if (numbers.size() > 0)
        return numbers[numbers.size() - 1];
        else
        {
            return -1;
        }
    }
    SDL_Keycode KeyManager::getLater(double m, double n) //of m and n, finds which key which pressed later/recently.
    {
        int size = numbers.size();
        for(int i = size-1; i >= 0; i --)
        {
            if (numbers[i] == m || numbers[i] == n)
                {
                    return numbers[i];
                }
        }
        return -1;
    }
    void KeyManager::addNumber(double key)
    {
        numbers.push_back(key);
    }
    int KeyManager::findNumber(double n) //finds the index of n or -1 if n isn't found
    {
        for (int i = 0; i < numbers.size(); i ++)
        {
            if (numbers[i] == n )
            {
                return i;
            }
        }
        return -1;
    }

    void KeyManager::getKeys(SDL_Event& e)
    {
        int sym = e.key.keysym.sym;
        if (e.type == SDL_KEYDOWN)
        {
            if (findNumber(sym) == -1)
            {
                addNumber(sym);
                justPressed = sym;
            }
            else
            {
                justPressed = -1;
            }
        }
        else
        {
            justPressed = -1;
            if (e.type == SDL_KEYUP)
            {
                removeNumber(sym);
            }
        }

    }
    SDL_Keycode KeyManager::getJustPressed()
    {
        return justPressed;
    }
bool MouseManager::left = false;
bool MouseManager::right = false;
bool MouseManager::middle = false;
int MouseManager::justClicked = -1;
bool* MouseManager::getButton(int key)
{
    switch (key)
    {
    case SDL_BUTTON_LEFT:
        return &left;
        break;
    case SDL_BUTTON_RIGHT:
        return &right;
        break;
    case SDL_BUTTON_MIDDLE:
        return &middle;
        break;
    }
}
void MouseManager::update(SDL_Event& e)
{
    if (e.type == SDL_MOUSEBUTTONDOWN)
    {
        bool* ptr = getButton(e.button.button);
        if (*ptr == false)
        {
            justClicked = e.button.button;
            *ptr = true;
        }
        else
        {
            justClicked = -1;
        }
    }
    else
    {
        justClicked = -1;
        if (e.type == SDL_MOUSEBUTTONUP)
        {
            *getButton(e.button.button) = false;
        }

    }
}
bool MouseManager::isPressed(int key)
{
    return *getButton(key);
}

int MouseManager::getJustClicked()
{
    return justClicked;
}
