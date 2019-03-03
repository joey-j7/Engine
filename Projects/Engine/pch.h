#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Engine/Common.h"
#include "Engine/Math.h"
#include "Engine/Logger/Log.h"
#include "Engine/Timer.h"

#include "Engine/DeltaTime.h"

#ifdef CB_PLATFORM_WINDOWS
	#include <Windows.h>
#endif