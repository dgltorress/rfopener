// Keys.h : declarations for special keys

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
        case LEFT_ARROW:
        case UP_ARROW:
            return true;
        default:
            return false;
    }
}

boolean Keys::isForwardKey(const int c) {
    switch (c) {
        case RIGHT_ARROW:
        case DOWN_ARROW:
            return true;
        default:
            return false;
    }
}
int Keys::refreshArrow(const int c) {
    if (c == ARROW_PREFIX) {
        return _getch();
    }
    return c;
}
