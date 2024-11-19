#include <sutk/AnimationEngine.hpp>
#include <sutk/UIDriver.hpp>

namespace SUTK
{
	AnimationEngine::AnimationEngine(UIDriver& driver) noexcept : UIDriverObject(driver), Runnable(driver)
	{

	}

	bool AnimationEngine::isRunning()
	{
		return (m_animContexts.size() != 0) || (m_animGroups.size() != 0);
	}

	void AnimationEngine::update()
	{
		m_animContexts.traverse([this](AnimContextBase* ctx) noexcept
		{
			// Publish on start event for the first time (first frame of the animation)
			if(!ctx->m_isStarted)
			{
				ctx->m_isStarted = com::True;
				ctx->onStart();
			}

			// Feed delta time to the stepping function (this also calls onStep() override in AnimContextBase)
			if(ctx->step(getRunnableUIDriver().getDeltaTime()))
			{
				ctx->m_isStarted = com::False;
				ctx->onEnd();
				// Now this animation has been completed, and should be now be destroyed
				this->m_animContexts.eraseCurrent();
				delete ctx;
			}
		});
		for(AnimGroup* group : m_animGroups)
		{
			auto& anims = group->getAnims();
			anims.traverse([](AnimContextBase* ctx) noexcept
			{
				// Publish on start event for the first time (first frame of the animation)
				if(!ctx->m_isStarted)
				{
					ctx->m_isStarted = com::True;
					ctx->onStart();
				}

				// Feed delta time to the stepping function (this also calls onStep() override in AnimContextBase)
				if(ctx->step(ctx->getUIDriver().getDeltaTime()))
				{
					ctx->m_isStarted = com::False;
					ctx->onEnd();
					// Now this animation has been completed, and should be now be destroyed
					ctx->m_group->getAnims().eraseCurrent();
					ctx->m_group = com::null_pointer<AnimGroup>();
					delete ctx;
				}
			});
		}
	}
}