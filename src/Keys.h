// Keys.h : special keys

#pragma once

#ifndef KEYS_H_
#define KEYS_H_

class Keys {
    private:
    public:
        // @return true if Enter or Space
        static boolean isConfirmKey(const int);
        // @return true if Esc or Back
        static boolean isExitKey(const int);
        // @return true if Left arrow, Up arrow, Numpad4 or Numpd8
        static boolean isBackKey(const int);
        // @return true if Right arrow, Down arrow, Numpad6 or Numpad2
        static boolean isForwardKey(const int);
};
#endif