#pragma once


#include <algorithm>
#include <atomic>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <chrono>
#include <exception>
#include <initializer_list>
#include <iterator>
#include <list>
#include <new>
#include <numeric>
#include <map>
#include <memory>
#include <mutex>
#include <cmath>
#include <set>
#include <stdexcept>
#include <string>
#include <stop_token>
#include <system_error>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <thread>
#include <utility>
#include <vector>
#include <functional>
#include <format>
#include <unordered_map>

#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <unordered_set>
#include <unordered_map>

#include <string>
#include <concepts>
#include <ranges>

#define NOMINMAX

#include <Windows.h>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "rpcrt4.lib")


#include <concurrent_queue.h>

//fmt
#include <nlohmann/json.hpp>

// custom develop

#include <convert_total.h>
#include <debug_console.h>
#include <simple_mesh_LH.h>
#include <simple_mesh_ext.h>



//Assimp

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <Assimp/postprocess.h>
#include <assimp/cimport.h>

#include <assimp/ai_assert.h>
#include <assimp/aabb.h>
#include <assimp/color4.h>
#include <assimp/quaternion.h>
#include <assimp/vector3.h>
#include <assimp/vector2.h>
#include <assimp/matrix4x4.h>
#include <assimp/matrix3x3.h>


#ifdef _DEBUG
#pragma comment(lib, "assimp-vc143-mtd.lib")
#else
#pragma comment(lib, "assimp-vc143-mt.lib")
#endif



//direct12

#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_4.h>

#include <wrl.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>

#include <DirectXTex.h>

#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3dcompiler")

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex_Debug.lib")
#else
#pragma comment(lib, "DirectXTex.lib")
#endif



//imgui
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

#ifdef _DEBUG
#include <dxgidebug.h>
#endif


#include "imgui_internal.h"



//fmod
#ifdef _DEBUG
#pragma comment(lib, "fmodL_vc.lib")
#pragma comment(lib, "fmodstudioL_vc.lib")
#else
#pragma comment(lib, "fmod_vc.lib")
#pragma comment(lib, "fmodstudio_vc.lib")
#endif


#include "fmod/fmod.hpp"
#include "fmod/fmod_studio.hpp"
#include "fmod/fmod_errors.h"
#include "fmod/fmod_dsp.h"

#include <SDKDDKVer.h>

#include <class_define.h>

//여긴 자명한 외부 라이브러리 + 전방선언만.

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;

namespace dx = DirectX;

using namespace dxe;