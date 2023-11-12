// Keys.h : special keys

#pragma once

#ifndef KEYS_H_
#define KEYS_H_

#include <Windows.h>
#include <conio.h>

class Keys {
    private:
        static const int ARROW_PREFIX = 224;
        static const int UP_ARROW = 72;
        static const int RIGHT_ARROW = 77;
        static const int DOWN_ARROW = 80;
        static const int LEFT_ARROW = 75;
    public:
        // @return true if Enter or Space
        static boolean isConfirmKey(const int);
        // @return true if Esc or Back
        static boolean isExitKey(const int);
        // @return true if Left arrow, Up arrow
        static boolean isBackKey(const int);
        // @return true if Right arrow, Down arrow
        static boolean isForwardKey(const int);
        // Performs `_getch()` if the character is an arrow
        static int refreshArrow(const int);
};
#endif