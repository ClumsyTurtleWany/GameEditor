#include "TimelineSystem.h"
#include "TimelineComponent.h"

void TimelineSystem::Tick(ECS::World* world, float time)
{
	for (auto& entity : world->GetEntities<TimelineComponent>())
	{
		auto timeline = entity->GetComponent<TimelineComponent>();

		if (timeline != nullptr)
		{
			
			timeline->Update(time);
		}
	}

}