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
        // @return true if Left arrow or Up arrow
        static boolean isBackKey(const int);
        // @return true if Right arrow or Down arrow
        static boolean isForwardKey(const int);
        // @return Actual arrow value if the character is identified as an arrow prefix, or the same one
        static int refreshArrow(const int);
};
#endif