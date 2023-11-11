// Keys.h : declarations for special keys

#include <Windows.h>

#include "Keys.h"

boolean Keys::isConfirmKey(const int c) {
    switch (c) {
        case VK_RETURN:
        case VK_SPACE:
            return true;
        default:
            return false;
    }
}

boolean Keys::isExitKey(const int c) {
    switch (c) {
        case VK_ESCAPE:
        case VK_BACK:
            return true;
        default:
            return false;
    }
}

boolean Keys::isBackKey(const int c) {
    switch (c) {
        case VK_LEFT:
        case VK_UP:
        case VK_NUMPAD4:
        case VK_NUMPAD8:
            return true;
        default:
            return false;
    }
}

boolean Keys::isForwardKey(const int c) {
    switch (c) {
        case VK_RIGHT:
        case VK_DOWN:
        case VK_NUMPAD6:
        case VK_NUMPAD2:
            return true;
        default:
            return false;
    }
}