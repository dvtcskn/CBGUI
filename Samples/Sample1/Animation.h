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
#pragma once

#include "cbClassBody.h"
#include "cbMath.h"
#include <iostream>
#include <functional>
#include <map>
#include <cbImage.h>
#include <cbWidget.h>

using namespace cbgui;

struct AnimationTransform
{
	float X, Y;
	float Roll, Yaw, Pitch;

	AnimationTransform()
		: X(0.0f)
		, Y(0.0f)
		, Roll(0.0f)
		, Yaw(0.0f)
		, Pitch(0.0f)
	{}
};

class IAnimationGraph
{
	cbBaseClassBody(cbClassDefaultProtectedConstructor, IAnimationGraph)
public:
	virtual AnimationTransform Get(const float ElapsedTime) const = 0;
	virtual AnimationTransform GetKeyFrame(const float KeyFrame) const = 0;
	virtual float GetLastKeyFrameTime() const = 0;
};

class AnimationGraph : public IAnimationGraph
{
	cbClassBody(cbClassConstructor, AnimationGraph, IAnimationGraph)
public:
	AnimationGraph() = default;
	AnimationGraph(cbWidget * Widget);
	AnimationGraph(const cbTransform& Transform);
	AnimationGraph(const AnimationTransform & Transform);
	AnimationGraph(const cbVector& Location, const cbVector3& RPY);

	virtual ~AnimationGraph()
	{
		Graph.clear();
	}

public:
	virtual AnimationTransform Get(const float ElapsedTime) const override
	{
		float PastKeyFrame = 0.0f;
		AnimationTransform PastTransform;
		for (const auto& KeyFrame : Graph)
		{
			if (ElapsedTime < KeyFrame.first)
			{
				AnimationTransform Transform = KeyFrame.second;
				const float Time = ElapsedTime - PastKeyFrame;
				const float KeyFrameTime = KeyFrame.first - PastKeyFrame;
				float Percent = Time / KeyFrameTime;

				if (Percent + 0.001f >= 1.0f)
					Percent = 1.0f;

				//Transform.X *= Percent;
				//Transform.Y *= Percent;
				//Transform.Roll *= Percent;
				//Transform.Yaw *= Percent;
				//Transform.Pitch *= Percent;
				Transform.Roll = cbgui::Lerp(PastTransform.Roll, Transform.Roll, Percent);
				Transform.Yaw = cbgui::Lerp(PastTransform.Yaw, Transform.Yaw, Percent);
				Transform.Pitch = cbgui::Lerp(PastTransform.Pitch, Transform.Pitch, Percent);
				Transform.X = cbgui::Lerp(PastTransform.X, Transform.X, Percent);
				Transform.Y = cbgui::Lerp(PastTransform.Y, Transform.Y, Percent);
				//Transform.ScaleX *= Percent;
				//Transform.ScaleY *= Percent;
				return Transform;
			}
			PastKeyFrame = KeyFrame.first;
			PastTransform = KeyFrame.second;
		}
		return PastTransform;
	}
	virtual AnimationTransform GetKeyFrame(const float KeyFrame) const override
	{
		if (Graph.find(KeyFrame) != Graph.end())
			return Graph.at(KeyFrame);
		return AnimationTransform();
	}
	virtual void SetKeyFrame(const float KeyFrame, const AnimationTransform& Transform)
	{
		if (Graph.find(KeyFrame) != Graph.end())
			Graph.insert({ KeyFrame, Transform });
		else
			Graph[KeyFrame] = Transform;
	}

	virtual void SetLocationKeyFrame(const float KeyFrame, const float X, const float Y)
	{
		if (Graph.find(KeyFrame) != Graph.end())
		{
			Graph[KeyFrame].X = X;
			Graph[KeyFrame].Y = Y;
		}
		else
		{
			AnimationTransform Transform;
			Transform.X = X;
			Transform.Y = Y;
			Graph.insert({ KeyFrame, Transform });
		}
	}
	virtual void SetRotationKeyFrame(const float KeyFrame, const float Roll, const float Yaw, const float Pitch)
	{
		if (Graph.find(KeyFrame) != Graph.end())
		{
			Graph[KeyFrame].Roll = Roll;
			Graph[KeyFrame].Yaw = Yaw;
			Graph[KeyFrame].Pitch = Pitch;
		}
		else
		{
			AnimationTransform Transform;
			Transform.Roll = Roll;
			Transform.Yaw = Yaw;
			Transform.Pitch = Pitch;
			Graph.insert({ KeyFrame, Transform });
		}
	}

	virtual float GetLastKeyFrameTime() const override
	{
		return std::prev(Graph.end())->first;
	}

	std::map<float, AnimationTransform> Graph;
};

class IAnimationTimeline
{
	cbBaseClassBody(cbClassDefaultProtectedConstructor, IAnimationTimeline)
public:
	virtual AnimationTransform GetCurrentTransform() const = 0;
	virtual void SetGraph(IAnimationGraph* AnimationGraph) = 0;
	virtual void Run(const float DeltaTime) = 0;
	virtual void Play() = 0;
	virtual void Pause() = 0;
	virtual void Reset() = 0;
	virtual void ForceEndAnimation() = 0;
	virtual bool IsAnimationEnded() const = 0;
	virtual bool IsAnimationActive() const = 0;
	virtual float GetElapsedTime() const = 0;

	virtual void SetName(const std::string pName) = 0;
	virtual std::string GetName() const = 0;
};

template<class T>
class AnimationTimeline : public IAnimationTimeline
{
	cbClassBody(cbClassConstructor, AnimationTimeline, IAnimationTimeline)
public:
	AnimationTimeline(const std::string pName, T* pWidget)
		: Name(pName)
		, Widget(pWidget)
		, ElapsedTime(0.0f)
		, Graph(nullptr)
		, bIsAnimationActive(false)
		, bIsAnimationInLoop(false)
	{}

	virtual ~AnimationTimeline()
	{
		delete Graph;
		Graph = nullptr;
		Widget = nullptr;
	}

	virtual void SetName(const std::string pName) override { Name = pName; }
	virtual std::string GetName() const override { return Name; }

	virtual AnimationTransform GetCurrentTransform() const override
	{
		if (Graph)
			return Graph->Get(ElapsedTime);
		return AnimationTransform();
	}
	virtual void SetGraph(IAnimationGraph* AnimationGraph) override
	{
		Graph = AnimationGraph;
	}
	virtual void Run(const float DeltaTime) override
	{
		if (!bIsAnimationActive)
			return;

		const bool bIsAnimationEnded = IsAnimationEnded();

		if (bIsAnimationInLoop && bIsAnimationEnded)
		{
			Reset();
			Play();
		}
		else if (bIsAnimationEnded)
		{
			bIsAnimationActive = false;
		}

		if (Graph && !bIsAnimationEnded)
		{
			ElapsedTime += DeltaTime;
			AnimationTransform Transform = Graph->Get(ElapsedTime);
			Widget->SetLocation(cbVector(Transform.X, Transform.Y));
			Widget->SetRotation(Transform.Roll);
		}
	}
	virtual void Play() override
	{
		bIsAnimationActive = true;
	}
	virtual void Pause() override
	{
		bIsAnimationActive = false;
	}
	virtual void Reset() override
	{
		ElapsedTime = 0.0f;
	}
	virtual bool IsAnimationEnded() const override
	{
		if (Graph)
			return ElapsedTime >= Graph->GetLastKeyFrameTime();
		return true;
	}
	virtual float GetElapsedTime() const override
	{
		return ElapsedTime;
	}
	virtual bool IsAnimationActive() const override
	{
		return bIsAnimationActive;
	}
	virtual void ForceEndAnimation()
	{
		bIsAnimationActive = false;
		ElapsedTime = Graph->GetLastKeyFrameTime();

		AnimationTransform Transform = Graph->Get(ElapsedTime);
		Widget->SetLocation(cbVector(Transform.X, Transform.Y));
		Widget->SetRotation(Transform.Roll);
	}
	virtual void SetLoop(bool value)
	{
		bIsAnimationInLoop = value;
		if (bIsAnimationInLoop && IsAnimationEnded())
		{
			Reset();
			Play();
		}
	}
protected:
	bool bIsAnimationInLoop;
	bool bIsAnimationActive;
	float ElapsedTime;
	IAnimationGraph* Graph;
	T* Widget;
	std::string Name;
};

class cbAnimation
{
	cbBaseClassBody(cbClassConstructor, cbAnimation)
public:
	cbAnimation();
	virtual ~cbAnimation();
public:
	virtual void BeginPlay();
	virtual void Tick(const float DeltaTime);

	virtual void AddTimeline(IAnimationTimeline* Graph);
	virtual void StartAnimations();
	virtual void EndAnimations();
	virtual void ResetAnimations();

	std::vector<IAnimationTimeline*> GetTimelines() const { return Timelines; }
	IAnimationTimeline* GetTimeline(std::string Name) const;
	void RemoveTimeline(std::string Name);

protected:
	std::vector<IAnimationTimeline*> Timelines;
};
