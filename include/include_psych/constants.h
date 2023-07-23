#pragma once
#include <unordered_map>
namespace Keys {
    std::unordered_map<std::string, int> stringToGLFWKeys = {
    {"A", GLFW_KEY_A},
    {"B", GLFW_KEY_B},
    {"C", GLFW_KEY_C},
    {"D", GLFW_KEY_D},
    {"E", GLFW_KEY_E},
    {"F", GLFW_KEY_F},
    {"G", GLFW_KEY_G},
    {"H", GLFW_KEY_H},
    {"I", GLFW_KEY_I},
    {"J", GLFW_KEY_J},
    {"K", GLFW_KEY_K},
    {"L", GLFW_KEY_L},
    {"M", GLFW_KEY_M},
    {"N", GLFW_KEY_N},
    {"O", GLFW_KEY_O},
    {"P", GLFW_KEY_P},
    {"Q", GLFW_KEY_Q},
    {"R", GLFW_KEY_R},
    {"S", GLFW_KEY_S},
    {"T", GLFW_KEY_T},
    {"U", GLFW_KEY_U},
    {"V", GLFW_KEY_V},
    {"W", GLFW_KEY_W},
    {"X", GLFW_KEY_X},
    {"Y", GLFW_KEY_Y},
    {"Z", GLFW_KEY_Z},
    {"0", GLFW_KEY_0},
    {"1", GLFW_KEY_1},
    {"2", GLFW_KEY_2},
    {"3", GLFW_KEY_3},
    {"4", GLFW_KEY_4},
    {"5", GLFW_KEY_5},
    {"6", GLFW_KEY_6},
    {"7", GLFW_KEY_7},
    {"8", GLFW_KEY_8},
    {"9", GLFW_KEY_9},
    {"SPACE", GLFW_KEY_SPACE},
    {"ESCAPE", GLFW_KEY_ESCAPE},
    {"ENTER", GLFW_KEY_ENTER},
    {"TAB", GLFW_KEY_TAB},
    {"BACKSPACE", GLFW_KEY_BACKSPACE},
    {"INSERT", GLFW_KEY_INSERT},
    {"DELETE", GLFW_KEY_DELETE},
    {"RIGHT", GLFW_KEY_RIGHT},
    {"LEFT", GLFW_KEY_LEFT},
    {"DOWN", GLFW_KEY_DOWN},
    {"UP", GLFW_KEY_UP},
    {"PAGE_UP", GLFW_KEY_PAGE_UP},
    {"PAGE_DOWN", GLFW_KEY_PAGE_DOWN},
    {"HOME", GLFW_KEY_HOME},
    {"END", GLFW_KEY_END},
    {"CAPS_LOCK", GLFW_KEY_CAPS_LOCK},
    {"SCROLL_LOCK", GLFW_KEY_SCROLL_LOCK},
    {"NUM_LOCK", GLFW_KEY_NUM_LOCK},
    {"PRINT_SCREEN", GLFW_KEY_PRINT_SCREEN},
    {"PAUSE", GLFW_KEY_PAUSE},
    {"F1", GLFW_KEY_F1},
    {"F2", GLFW_KEY_F2},
    {"F3", GLFW_KEY_F3},
    {"F4", GLFW_KEY_F4},
    {"F5", GLFW_KEY_F5},
    {"F6", GLFW_KEY_F6},
    {"F7", GLFW_KEY_F7},
    {"F8", GLFW_KEY_F8},
    {"F9", GLFW_KEY_F9},
    {"F10", GLFW_KEY_F10},
    {"F11", GLFW_KEY_F11},
    {"F12", GLFW_KEY_F12},
    {"KP_0", GLFW_KEY_KP_0},
    {"KP_1", GLFW_KEY_KP_1},
    {"KP_2", GLFW_KEY_KP_2},
    {"KP_3", GLFW_KEY_KP_3},
    {"KP_4", GLFW_KEY_KP_4},
    {"KP_5", GLFW_KEY_KP_5},
    {"KP_6", GLFW_KEY_KP_6},
    {"KP_7", GLFW_KEY_KP_7},
    {"KP_8", GLFW_KEY_KP_8},
    {"KP_9", GLFW_KEY_KP_9},
    {"KP_DECIMAL", GLFW_KEY_KP_DECIMAL},
    {"KP_DIVIDE", GLFW_KEY_KP_DIVIDE},
    {"KP_MULTIPLY", GLFW_KEY_KP_MULTIPLY},
    {"KP_SUBTRACT", GLFW_KEY_KP_SUBTRACT},
    {"KP_ADD", GLFW_KEY_KP_ADD},
    {"KP_ENTER", GLFW_KEY_KP_ENTER},
    {"KP_EQUAL", GLFW_KEY_KP_EQUAL},
    {"LEFT_SHIFT", GLFW_KEY_LEFT_SHIFT},
    {"LEFT_CONTROL", GLFW_KEY_LEFT_CONTROL},
    {"LEFT_ALT", GLFW_KEY_LEFT_ALT},
    {"LEFT_SUPER", GLFW_KEY_LEFT_SUPER},
    {"RIGHT_SHIFT", GLFW_KEY_RIGHT_SHIFT},
    {"RIGHT_CONTROL", GLFW_KEY_RIGHT_CONTROL},
    {"RIGHT_ALT", GLFW_KEY_RIGHT_ALT},
    {"RIGHT_SUPER", GLFW_KEY_RIGHT_SUPER},
    {"MENU", GLFW_KEY_MENU},
    {"LAST", GLFW_KEY_LAST},
    { "LEFT_CLICK",GLFW_MOUSE_BUTTON_LEFT},
    { "RIGHT_CLICK",GLFW_MOUSE_BUTTON_RIGHT  },
    { "MIDDLE_CLICK",GLFW_MOUSE_BUTTON_MIDDLE }
    };
    std::unordered_map<int,std::string> GLFWKeysToString = {
        {GLFW_KEY_SPACE, "SPACE"},
        {GLFW_KEY_APOSTROPHE, "APOSTROPHE"},
        {GLFW_KEY_COMMA, "COMMA"},
        {GLFW_KEY_MINUS, "MINUS"},
        {GLFW_KEY_PERIOD, "PERIOD"},
        {GLFW_KEY_SLASH, "SLASH"},
        {GLFW_KEY_0, "0"},
        {GLFW_KEY_1, "1"},
        {GLFW_KEY_2, "2"},
        {GLFW_KEY_3, "3"},
        {GLFW_KEY_4, "4"},
        {GLFW_KEY_5, "5"},
        {GLFW_KEY_6, "6"},
        {GLFW_KEY_7, "7"},
        {GLFW_KEY_8, "8"},
        {GLFW_KEY_9, "9"},
        {GLFW_KEY_SEMICOLON, "SEMICOLON"},
        {GLFW_KEY_EQUAL, "EQUAL"},
        {GLFW_KEY_A, "A"},
        {GLFW_KEY_B, "B"},
        {GLFW_KEY_C, "C"},
        {GLFW_KEY_D, "D"},
        {GLFW_KEY_E, "E"},
        {GLFW_KEY_F, "F"},
        {GLFW_KEY_G, "G"},
        {GLFW_KEY_H, "H"},
        {GLFW_KEY_I, "I"},
        {GLFW_KEY_J, "J"},
        {GLFW_KEY_K, "K"},
        {GLFW_KEY_L, "L"},
        {GLFW_KEY_M, "M"},
        {GLFW_KEY_N, "N"},
        {GLFW_KEY_O, "O"},
        {GLFW_KEY_P, "P"},
        {GLFW_KEY_Q, "Q"},
        {GLFW_KEY_R, "R"},
        {GLFW_KEY_S, "S"},
        {GLFW_KEY_T, "T"},
        {GLFW_KEY_U, "U"},
        {GLFW_KEY_V, "V"},
        {GLFW_KEY_W, "W"},
        {GLFW_KEY_X, "X"},
        {GLFW_KEY_Y, "Y"},
        {GLFW_KEY_Z, "Z"},
        {GLFW_KEY_LEFT_BRACKET, "LEFT_BRACKET"},
        {GLFW_KEY_BACKSLASH, "BACKSLASH"},
        {GLFW_KEY_RIGHT_BRACKET, "RIGHT_BRACKET"},
        {GLFW_KEY_GRAVE_ACCENT, "GRAVE_ACCENT"},
        {GLFW_KEY_WORLD_1, "WORLD_1"},
        {GLFW_KEY_WORLD_2, "WORLD_2"},
        {GLFW_KEY_ESCAPE, "ESCAPE"},
        {GLFW_KEY_ENTER, "ENTER"},
        {GLFW_KEY_TAB, "TAB"},
        {GLFW_KEY_BACKSPACE, "BACKSPACE"},
        {GLFW_KEY_INSERT, "INSERT"},
        {GLFW_KEY_DELETE, "DELETE"},
        {GLFW_KEY_RIGHT, "RIGHT"},
        {GLFW_KEY_LEFT, "LEFT"},
        {GLFW_KEY_DOWN, "DOWN"},
        {GLFW_KEY_UP, "UP"},
        {GLFW_KEY_PAGE_UP, "PAGE_UP"},
        {GLFW_KEY_PAGE_DOWN, "PAGE_DOWN"},
        {GLFW_KEY_HOME, "HOME"},
        {GLFW_KEY_END, "END"},
        {GLFW_KEY_CAPS_LOCK, "CAPS_LOCK"},
        {GLFW_KEY_SCROLL_LOCK, "SCROLL_LOCK"},
        {GLFW_KEY_NUM_LOCK, "NUM_LOCK"},
        {GLFW_KEY_PRINT_SCREEN, "PRINT_SCREEN"},
        {GLFW_KEY_PAUSE, "PAUSE"},
        {GLFW_KEY_F1, "F1"},
        {GLFW_KEY_F2, "F2"},
        {GLFW_KEY_F3, "F3"},
        {GLFW_KEY_F4, "F4"},
        {GLFW_KEY_F5, "F5"},
        {GLFW_KEY_F6, "F6"},
        {GLFW_KEY_F7, "F7"},
        {GLFW_KEY_F8, "F8"},
        {GLFW_KEY_F9, "F9"},
        {GLFW_KEY_F10, "F10"},
        {GLFW_KEY_F11, "F11"},
        {GLFW_KEY_F12, "F12"},
        {GLFW_KEY_F13, "F13"},
        {GLFW_KEY_F14, "F14"},
        {GLFW_KEY_F15, "F15"},
        {GLFW_KEY_F16, "F16"},
        {GLFW_KEY_F17, "F17"},
        {GLFW_KEY_F18, "F18"},
        {GLFW_KEY_F19, "F19"},
        {GLFW_KEY_F20, "F20"},
        {GLFW_KEY_F21, "F21"},
        {GLFW_KEY_F22, "F22"},
        {GLFW_KEY_F23, "F23"},
        {GLFW_KEY_F24, "F24"},
        {GLFW_KEY_F25, "F25"},
        {GLFW_KEY_KP_0, "KP_0"},
        {GLFW_KEY_KP_1, "KP_1"},
        {GLFW_KEY_KP_2, "KP_2"},
        {GLFW_KEY_KP_3, "KP_3"},
        {GLFW_KEY_KP_4, "KP_4"},
        {GLFW_KEY_KP_5, "KP_5"},
        {GLFW_KEY_KP_6, "KP_6"},
        {GLFW_KEY_KP_7, "KP_7"},
        {GLFW_KEY_KP_8, "KP_8"},
        {GLFW_KEY_KP_9, "KP_9"},
        {GLFW_KEY_KP_DECIMAL, "KP_DECIMAL"},
        {GLFW_KEY_KP_DIVIDE, "KP_DIVIDE"},
        {GLFW_KEY_KP_MULTIPLY, "KP_MULTIPLY"},
        {GLFW_KEY_KP_SUBTRACT, "KP_SUBTRACT"},
        {GLFW_KEY_KP_ADD, "KP_ADD"},
        {GLFW_KEY_KP_ENTER, "KP_ENTER"},
        {GLFW_KEY_KP_EQUAL, "KP_EQUAL"},
        {GLFW_KEY_LEFT_SHIFT, "LEFT_SHIFT"},
        {GLFW_KEY_LEFT_CONTROL, "LEFT_CONTROL"},
        {GLFW_KEY_LEFT_ALT, "LEFT_ALT"},
        {GLFW_KEY_LEFT_SUPER, "LEFT_SUPER"},
        {GLFW_KEY_RIGHT_SHIFT, "RIGHT_SHIFT"},
        {GLFW_KEY_RIGHT_CONTROL, "RIGHT_CONTROL"},
        {GLFW_KEY_RIGHT_ALT, "RIGHT_ALT"},
        {GLFW_KEY_RIGHT_SUPER, "RIGHT_SUPER"},
        {GLFW_KEY_MENU, "MENU"},
        { GLFW_MOUSE_BUTTON_LEFT ,"LEFT_CLICK"},
        { GLFW_MOUSE_BUTTON_RIGHT ,"RIGHT_CLICK" },
        { GLFW_MOUSE_BUTTON_MIDDLE ,"MIDDLE_CLICK" }
    };
}