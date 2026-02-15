#include "SceneManager.h"
#include "Scene.h"

void dae::SceneManager::Update(float delta_time)
{
	for(auto& scene : m_scenes)
	{
		scene->Update(delta_time);
	}
}

void dae::SceneManager::Render()
{
	for (const auto& scene : m_scenes)
	{
		scene->Render();
	}
}

dae::Scene& dae::SceneManager::CreateScene()
{
	m_scenes.emplace_back(new Scene());
	return *m_scenes.back();
}
