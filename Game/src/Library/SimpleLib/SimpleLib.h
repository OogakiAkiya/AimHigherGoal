//=========================================================
//
// Simple Library
//  ２年生シェーダ授業用のDirectX9簡易ライブラリ
//
// SimpleLib.h
//
// 制作者：鎌田
//
//=========================================================

#ifndef SimpleLib_h
#define SimpleLib_h

//===============================================
//
// 基本
//
//===============================================
#pragma comment(lib,"winmm.lib")

#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <math.h>
#include <commctrl.h>
#include <ctime>

//===============================================
//
// DirectX
//
//===============================================
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dsound.lib")

#pragma warning(disable : 4996)
#pragma warning(disable : 4819)

#include "d3d9.h"
#include "d3dx9.h"
#include "dsound.h"


//===============================================
//
// STL
//
//===============================================
#include <map>
#include <unordered_map>
#include <map>
#include <string>
#include <array>
#include <vector>
#include <stack>
#include <list>
#include <iterator>
#include <queue>
#include <algorithm>
#include <memory>
#include <random>
#include <fstream>
#include <iostream>
#include <sstream>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <future>

//===============================================
// SimpleLib
//	2年生授業用の簡易DirectX9操作クラス群]
//===============================================
#include "Helper.h"
#include "CMTRand.h"
#include "CVector3.h"
#include "CMatrix.h"
#include "CQuat.h"
#include "CTexture.h"
#include "CDirectXGraphics.h"
#include "CSurface.h"
#include "CMesh.h"

#include "SkinMesh/SkinMeshLoader.h"
#include "SkinMesh/KeyframeAnimation.h"
#include "SkinMesh/CSkinMesh.h"
#include "SkinMesh/CBoneController.h"


#endif
