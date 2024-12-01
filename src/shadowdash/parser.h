#pragma once
#include "manifest.h"
#include "../state.h"

namespace shadowdash {
void AddBuildToNinjaState(const build& b, State* ninja_state);
}