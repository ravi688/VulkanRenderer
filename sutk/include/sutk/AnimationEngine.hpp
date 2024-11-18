#pragma once

#include <sutk/defines.hpp> // for SUTK_API
#include <sutk/IRunnable.hpp> // for SUTK::Runnable
#include <sutk/UIDriverObject.hpp> // for SUTK::UIDriverObject
#include <sutk/UIDriver.hpp>

#include <common/Bool.hpp> // for com::Bool
#include <common/Event.hpp> // for com::Event
#include <common/UnorderedEraseSafeVectorProxy.hpp> // for com::UnorderedEraseSafeVectorProxy

#include <concepts> // for std::derived_from

namespace SUTK
{
	class SUTK_API AnimationEngine : public UIDriverObject, public Runnable
	{
	public:
		class AnimContextBase;
		class AnimGroup : public UIDriverObject
		{
			friend class AnimContextBase;
			u32 m_count;
			u32 m_stepCount;
		public:
			void present(AnimContextBase* animContext) noexcept
			{ 
				onPresent(animContext);
				onWhenAnyStart();
				m_count += 1; 
			}
			void absent(AnimContextBase* animContext) noexcept
			{ 
				onAbsent(animContext);
				_com_assert(m_count > 0); 
				m_count -= 1;
				if(!m_count)
					onWhenAllEnd();
			}
			void step() noexcept
			{
				m_stepCount += 1;
				if(m_stepCount == m_count)
				{
					onStepAll();
					m_stepCount = 0;
				}
			}
		protected:
			AnimGroup(UIDriver& driver) noexcept : UIDriverObject(driver), m_count(0), m_stepCount(0) { }
			virtual ~AnimGroup() noexcept = default;
			virtual void onPresent(AnimContextBase* animContext) noexcept { }
			virtual void onAbsent(AnimContextBase* animContext) noexcept { }
			virtual void onWhenAnyStart() noexcept { }
			virtual void onWhenAllEnd() noexcept { }
			virtual void onStepAll() noexcept { }
		};
		class AnimContextBase : public UIDriverObject
		{
			friend class AnimationEngine;
		private:
			AnimGroup* m_group;
			f32 m_curValue;
			f32 m_endValue;
			f32 m_speed;
			com::Bool m_isStarted;

			com::Bool step(f32 deltaTime) noexcept
			{
				f32 deltaValue = m_speed * deltaTime;
				f32 safeDeltaValue = m_endValue - m_curValue;
				if(std::abs(deltaValue) >= std::abs(safeDeltaValue))
				{
					deltaValue = safeDeltaValue;
					m_curValue = m_endValue;
				}
				else m_curValue += deltaValue;
				onStep(deltaValue);
				if(m_group)
					m_group->step();
				return com::Bool { m_curValue == m_endValue };
			}

		protected:
			// Must be called in the overriding method
			virtual void onStart() noexcept
			{
				if(m_group)
					m_group->present(this);
			}
			// Must be implemented by deriving class
			virtual void onStep(f32 deltaValue) noexcept = 0;
			// Must be called in the end of the overriding method
			virtual void onEnd() noexcept
			{ 
				if(m_group)
					m_group->absent(this);
			}

		public:

			AnimContextBase(UIDriver& driver, AnimGroup* group = com::null_pointer<AnimGroup>(), f32 length = 1.0f, f32 duration = 1.0f) noexcept : UIDriverObject(driver),
					m_group(group),
					m_curValue(0.0f),
					m_endValue(length),
					m_speed(length / duration),
					m_isStarted(com::False) { }
			virtual ~AnimContextBase() noexcept = default;

			f32 getLength() const noexcept { return m_endValue; }
			void setLength(f32 length) noexcept { m_endValue = length; }
			f32 getSpeed() const noexcept { return m_speed; }
			void setSpeed(f32 speed) noexcept { m_speed = speed; }

			com::Bool isStarted() noexcept { return m_isStarted; }

			void abort() noexcept
			{

				auto& animContexts = getUIDriver().getAnimationEngine().m_animContexts;

				auto it = std::find(animContexts.begin(), animContexts.end(), this);
				if(it != animContexts.end())
				{
					m_isStarted = com::False;
					onEnd();
					animContexts.erase(animContexts.indexToIterator(std::distance(animContexts.begin(), it)));
				}
				else
					// This should never happen!
					_com_assert(false);
			}
		};
	private:
		com::UnorderedEraseSafeVectorProxy<AnimContextBase*> m_animContexts;
	public:
		AnimationEngine(UIDriver& driver) noexcept;

		template<std::derived_from<AnimContextBase> AnimContextType, typename... Args>
		AnimContextType* dispatchAnimation(Args&&... args) noexcept;

		virtual bool isRunning() override;
		virtual void update() override;
	};

	template<std::derived_from<AnimationEngine::AnimContextBase> AnimContextType, typename... Args>
	AnimContextType* AnimationEngine::dispatchAnimation(Args&&... args) noexcept
	{
		AnimContextType* animContext = getUIDriver().createObject<AnimContextType>(std::forward<Args&&>(args)...);
		m_animContexts.push_back(animContext);
		return animContext;
	}
}