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
			friend class AnimationEngine;
		public:
			typedef com::UnorderedEraseSafeVectorProxy<AnimContextBase*> AnimContextBaseVector;
		private:
			AnimContextBaseVector m_anims;
			u32 m_count;
			u32 m_stepCount;
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
		public:
			AnimContextBaseVector& getAnims() noexcept { return m_anims; }
			const AnimContextBaseVector& getAnims() const noexcept { return m_anims; }
		protected:
			AnimGroup(UIDriver& driver) noexcept : UIDriverObject(driver), m_count(0), m_stepCount(0) { }
			virtual ~AnimGroup() noexcept = default;
			// Called whenever a new animation is added into this group
			virtual void onPresent(AnimContextBase* animContext) noexcept { }
			// Called whenever a animation in this group is ended (either voluntarily or by abort)
			virtual void onAbsent(AnimContextBase* animContext) noexcept { }
			// Called whenever a new animation is added (started)
			virtual void onWhenAnyStart() noexcept { }
			// Called when all the animations in this group have ended
			virtual void onWhenAllEnd() noexcept { }
			// Called after when onStep() is invoked over all animations in this group
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
				return com::Bool { m_curValue == m_endValue };
			}

		protected:
			// Must be called in the overriding method
			virtual void onStart() noexcept
			{
				m_isStarted = com::True;
				if(m_group)
					m_group->present(this);
			}
			// Must be implemented by deriving class
			virtual void onStep(f32 deltaValue) noexcept = 0;
			// Must be called in the end of the overriding method
			virtual void onEnd(com::Bool isAborted) noexcept
			{ 
				if(m_group && m_isStarted)
				{
					std::cout << "Absent has been called" << std::endl;
					m_group->absent(this);
				}
				m_isStarted = com::False;
			}

		public:

			AnimContextBase(UIDriver& driver, AnimGroup* group = com::null_pointer<AnimGroup>(), f32 length = 1.0f, f32 duration = 1.0f) noexcept : UIDriverObject(driver),
					m_group(group),
					m_curValue(0.0f),
					m_endValue(length),
					m_speed(length / duration),
					m_isStarted(com::False)
			{
				if(group)
					group->m_anims.push_back(this);
			}
			virtual ~AnimContextBase() noexcept
			{
				com_assert(COM_DESCRIPTION(m_group == com::null_pointer<AnimGroup>()), "AnimContext can't be destroyed without calling abort() or let it finish");
				auto& animContexts = getUIDriver().getAnimationEngine().m_animContexts;
				com_assert(COM_DESCRIPTION(!animContexts.contains(this)), "AnimContext can't be destroyed without calling abort(), or let it finish");
			}

			AnimGroup* getAnimGroup() noexcept { return m_group; }

			f32 getLength() const noexcept { return m_endValue; }
			void setLength(f32 length) noexcept { m_endValue = length; }
			f32 getSpeed() const noexcept { return m_speed; }
			void setSpeed(f32 speed) noexcept { m_speed = speed; }

			void abort() noexcept
			{
				if(m_group)
				{
					onEnd(com::True);
					// TODO: optimize this,
					// if abort() is called during traversal of m_group->m_anims
					// and if this animation equals to the current animation
					// then call eraseCurrent() which is more efficient than searching first and then erasing.
					m_group->m_anims.find_erase(this);
					m_group = com::null_pointer<AnimGroup>();
				}
				else
				{
					auto& animContexts = getUIDriver().getAnimationEngine().m_animContexts;

					auto it = std::find(animContexts.begin(), animContexts.end(), this);
					if(it != animContexts.end())
					{
						onEnd(com::True);
						animContexts.erase(animContexts.indexToIterator(std::distance(animContexts.begin(), it)));
					}
					else
						// This should never happen!
						_com_assert(false);
				}
			}
		};
	private:
		com::UnorderedEraseSafeVectorProxy<AnimContextBase*> m_animContexts;
		std::vector<AnimGroup*> m_animGroups;
	public:
		AnimationEngine(UIDriver& driver) noexcept;

		template<std::derived_from<AnimGroup> AnimGroupType, typename... Args>
		AnimGroupType* createAnimGroup(Args&&... args) noexcept;
		template<std::derived_from<AnimGroup> AnimGroupType>
		void destroyAnimGroup(AnimGroupType* animGroup) noexcept;

		template<std::derived_from<AnimContextBase> AnimContextType, typename... Args>
		AnimContextType* dispatchAnimation(Args&&... args) noexcept;

		virtual bool isRunning() override;
		virtual void update() override;
	};

	template<std::derived_from<AnimationEngine::AnimContextBase> AnimContextType, typename... Args>
	AnimContextType* AnimationEngine::dispatchAnimation(Args&&... args) noexcept
	{
		AnimContextType* animContext = getUIDriver().createObject<AnimContextType>(std::forward<Args&&>(args)...);
		if(!animContext->getAnimGroup())
			m_animContexts.push_back(animContext);
		return animContext;
	}

	template<std::derived_from<AnimationEngine::AnimGroup> AnimGroupType, typename... Args>
	AnimGroupType* AnimationEngine::createAnimGroup(Args&&... args) noexcept
	{
		AnimGroupType* animGroup = getUIDriver().createObject<AnimGroupType>(std::forward<Args&&>(args)...);
		m_animGroups.push_back(animGroup);
		return animGroup;
	}
	
	template<std::derived_from<AnimationEngine::AnimGroup> AnimGroupType>
	void AnimationEngine::destroyAnimGroup(AnimGroupType* animGroup) noexcept
	{
		com::find_erase(m_animGroups, animGroup);
		delete animGroup;
	}
}