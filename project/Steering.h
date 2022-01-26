#pragma once
#include "BaseBehaviour.h"
#include "Exam_HelperStructs.h"
#include <IExamInterface.h>


//class Steering
//{
//public:
//	Steering() = default;
//	virtual ~Steering() = default;
//
//	virtual SteeringPlugin_Output GetSteering(const IExamInterface* pInterface, const Elite::Vector2& target) = 0;
//
//protected:
//
//};
//
//
//class Flee final : public Steering
//{
//public:
//	Flee() = default;
//	~Flee() override = default;
//
//	SteeringPlugin_Output GetSteering(const IExamInterface* pInterface, const Elite::Vector2& target) override;
//
//private:
//
//};
//
//class Seek final : public Steering
//{
//public:
//	Seek() = default;
//	~Seek() override = default;
//
//	SteeringPlugin_Output GetSteering(const IExamInterface* pInterface, const Elite::Vector2& target) override;
//
//private:
//
//};



inline SteeringPlugin_Output Flee(const IExamInterface* pInterface, const Elite::Vector2& target)
{
	auto agent = pInterface->Agent_GetInfo();
	SteeringPlugin_Output steering{};
	steering.LinearVelocity = (target - agent.Position);
	steering.LinearVelocity = pInterface->NavMesh_GetClosestPathPoint(steering.LinearVelocity + agent.Position) - agent.Position;
	//steering.LinearVelocity = -pInterface->NavMesh_GetClosestPathPoint(target) ;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= agent.MaxLinearSpeed;
	//steering.LinearVelocity *= -1.f;
	return steering;
}

inline SteeringPlugin_Output Seek(const IExamInterface* pInterface, const Elite::Vector2& target)
{
	auto agent = pInterface->Agent_GetInfo();
	
	SteeringPlugin_Output steering{};
	steering.LinearVelocity = pInterface->NavMesh_GetClosestPathPoint(target) - agent.Position;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= agent.MaxLinearSpeed;
	steering.RunMode = false;

	return steering;
}

inline void Rotate(const float angle, const float maxAngularVelocity, SteeringPlugin_Output& steering)
{
	steering.AutoOrient = false;
	steering.AngularVelocity = Elite::Clamp(angle, -maxAngularVelocity, maxAngularVelocity);
}