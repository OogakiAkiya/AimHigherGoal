#include"../Include.h"
#include "SceneBase.h"

SceneBase::~SceneBase()
{
}


void SceneBase::Update()
{
}

void SceneBase::Render3D()
{
}

void SceneBase::Render2D()
{
}

Camera* SceneBase::GetCamera()
{
	return camera;
}

bool SceneBase::GetEndFlg()
{
	return endflg;
}
