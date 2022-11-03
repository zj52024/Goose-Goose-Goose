#pragma once

#include "offsets.hpp"
#include "utils.hpp"
#include "MinHook/include/MinHook.h"

DWORD_PTR getGameState();                       ////////////////// NOT GOOD....
bool GameManagerHook();