#include "Scene.h"
#include <algorithm>
#include "Scene.h"

using namespace dae;

void Scene::Add(std::unique_ptr<GameObject> object)
{
	assert(object != nullptr && "Cannot add a null GameObject to the scene.");

	m_objects.emplace_back(std::move(object));
}

void Scene::Remove(const GameObject& object)
{
	m_objects.erase(
		std::remove_if(
			m_objects.begin(),
			m_objects.end(),
			[&object](const auto& ptr) { return ptr.get() == &object; }
		),
		m_objects.end()
	);
}

void Scene::RemoveAll()
{
	m_objects.clear();
}

void dae::Scene::Cleanup()
{
	m_objects.erase(
		std::remove_if(
			m_objects.begin(),
			m_objects.end(),
			[](const auto& object)
			{
				return object->IsMarkedForDestroy();
			}),
		m_objects.end());
}

void Scene::Update(float delta_time)
{
	for(auto& object : m_objects)
	{
		object->Update(delta_time);
	}

	CheckCollisions();

	Cleanup();
}

void Scene::Render() const
{
	for (const auto& object : m_objects)
	{
		object->Render();
	}
}

void dae::Scene::CheckCollisions()
{
	std::vector<ColliderComponent*> colliders;

	for (auto& obj : m_objects)
	{
		if (auto* col = obj->GetComponent<ColliderComponent>())
			colliders.push_back(col);
	}

	for (size_t i = 0; i < colliders.size(); ++i)
	{
		for (size_t j = i + 1; j < colliders.size(); ++j)
		{
			// PREGUNTAR!!!
			auto* a = colliders[i];
			auto* b = colliders[j];

			a->CheckCollision(*b);
			b->CheckCollision(*a);
		}
	}
}

