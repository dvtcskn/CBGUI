/* ---------------------------------------------------------------------------------------
* MIT License
*
* Copyright (c) 2022 Davut Coþkun.
* All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
* ---------------------------------------------------------------------------------------
*/

#include "pch.h"
#include "Animation.h"
#include "cbWidget.h"

AnimationGraph::AnimationGraph(cbWidget* UI)
{
	AnimationTransform Transform;
	Transform.X = UI->GetLocation().X;
	Transform.Y = UI->GetLocation().Y;
	float Rotation = UI->GetRotation();
	Transform.Roll = Rotation;
	Transform.Pitch = 0.0f;
	Transform.Yaw = 0.0f;
	Graph.insert({ 0.0f, Transform });
}

AnimationGraph::AnimationGraph(const AnimationTransform& Transform)
{
	Graph.insert({ 0.0f, Transform });
}

AnimationGraph::AnimationGraph(const cbTransform& inTransform)
{
	AnimationTransform Transform;
	Transform.X = inTransform.GetCenter().X;
	Transform.Y = inTransform.GetCenter().Y;
	float Roll = inTransform.GetRotation();
	Transform.Roll = Roll;
	Transform.Pitch = 0.0f;
	Transform.Yaw = 0.0f;
	Graph.insert({ 0.0f, Transform });
}

AnimationGraph::AnimationGraph(const cbVector& Location, const cbVector3& RPY)
{
	AnimationTransform Transform;
	Transform.X = Location.X;
	Transform.Y = Location.Y;
	cbVector3 RollPitchYaw = RPY;
	Transform.Roll = RollPitchYaw.X;
	Transform.Pitch = RollPitchYaw.Y;
	Transform.Yaw = RollPitchYaw.Z;
	Graph.insert({ 0.0f, Transform });
}

cbAnimation::cbAnimation()
{
}

cbAnimation::~cbAnimation()
{
	for (auto& Timeline : Timelines)
	{
		delete Timeline;
		Timeline = nullptr;
	}
	Timelines.clear();
}

void cbAnimation::BeginPlay()
{
}

void cbAnimation::Tick(const float DeltaTime)
{
	for (const auto& Timeline : Timelines)
		Timeline->Run(DeltaTime);
}

IAnimationTimeline* cbAnimation::GetTimeline(std::string Name) const
{
	for (const auto& Timeline : Timelines)
	{
		if (Timeline->GetName() == Name)
			return Timeline;
	}
	return nullptr;
}

void cbAnimation::RemoveTimeline(std::string Name)
{
	std::vector<IAnimationTimeline*>::iterator it = Timelines.begin();
	while (it != Timelines.end())
	{
		if ((*it)->GetName() == Name)
		{
			it = Timelines.erase(it);
			break;
		}
		else {
			it++;
		}
	}
}

void cbAnimation::AddTimeline(IAnimationTimeline* Timeline)
{
	Timelines.push_back(Timeline);
}

void cbAnimation::StartAnimations()
{
	for (const auto& Timeline : Timelines)
		Timeline->Play();
}

void cbAnimation::EndAnimations()
{
	for (const auto& Timeline : Timelines)
		Timeline->ForceEndAnimation();
}

void cbAnimation::ResetAnimations()
{
	for (const auto& Timeline : Timelines)
		Timeline->Reset();
}
