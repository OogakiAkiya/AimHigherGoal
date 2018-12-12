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

std::shared_ptr<Camera> SceneBase::GetCamera()
{
	return camera;
}

bool SceneBase::GetEndFlg()
{
	return endflg;
}
