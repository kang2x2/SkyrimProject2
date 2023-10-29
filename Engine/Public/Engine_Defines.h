#pragma once

#pragma warning (disable : 4251)
#pragma warning (disable : 26812)
#include <d3d11.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXCollision.h>
#include <Effect11/d3dx11effect.h>
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/ScreenGrab.h>

#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/VertexTypes.h>
#include <DirectXTK/Effects.h>

#include <DirectXTK\SpriteBatch.h>
#include <DirectXTK\SpriteFont.h>

#include <assimp\scene.h>
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>

using namespace DirectX;

#include <iostream>
#include <vector> /* �迭 */
#include <list> /* ����ũ�帮��Ʈ */
#include <map> /* ����Ʈ�� */
#include <unordered_map> /* �ؽ����̺� */
#include <algorithm>

// imgui ���ڿ� ����
#include <string>
#include <atlstr.h>
#include <locale>
#include <codecvt>

#include "Engine_Typedef.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Function.h"

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG

using namespace Engine;
using namespace std;