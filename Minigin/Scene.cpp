#include "Scene.h"
#include <algorithm>
#include "Scene.h"

using namespace dae;

void Scene::Add(std::unique_ptr<GameObject> object)
{
	assert(object != nullptr && "Cannot add a null GameObject to the scene.");

	// if obj has a collider component save to later check for collisions
	if (auto* col = object->GetComponent<ColliderComponent>())
	{
		m_colliders.push_back(col);
	}

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
	for (auto& obj : m_objects)
	{
		if (obj->IsMarkedForDestroy())
		{
			if (auto* col = obj->GetComponent<ColliderComponent>())
			{
				std::erase(m_colliders, col);
			}
		}
	}


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
	for (size_t i = 0; i < m_colliders.size(); ++i)
	{
		for (size_t j = i + 1; j < m_colliders.size(); ++j)
		{
			auto* a = m_colliders[i];
			auto* b = m_colliders[j];

			a->CheckCollision(*b);

			//if (a->CheckCollision(*b))
			//{
			//	Event e(make_sdbm_hash("OnCollision"));

			//	a->GetSubject().NotifyObservers(e, b);
			//	b->GetSubject().NotifyObservers(e, a);
			//}
		}
	}
}

