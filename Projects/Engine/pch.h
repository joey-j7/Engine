#pragma once

#pragma warning(disable : 4275)
#pragma warning(disable : 4251)

#include <iostream>
#include <fstream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <cstdio>
#include <chrono>

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

/* GLFW */
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#if CB_RENDERING_API == CB_RENDERER_VULKAN && !defined(GLFW_INCLUDE_VULKAN)
#define GLFW_INCLUDE_VULKAN
#endif

#include <GLFW/glfw3.h>

/* Windows */
#ifdef CB_PLATFORM_WINDOWS
	#include <Windows.h>
#endif