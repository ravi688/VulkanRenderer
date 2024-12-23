#include <sutk/PassiveLabel.hpp>
#include <sutk/ContainerUtility.hpp> // for SUTK::ContainerUtility::findTextGroupHandle()

namespace SUTK
{
	PassiveLabel::PassiveLabel(UIDriver& driver, Container* parent, GfxDriverObjectHandleType textGroup) noexcept : Label(driver, parent), 
																										m_isReassociateOnParentChange(com::True)
	{
		// If no valid text group was assigned upon construction of this object
		// Then try to find a TextGroupContainer up the Container Hierarchy
		if(textGroup == GFX_DRIVER_OBJECT_NULL_HANDLE)
			textGroup = ContainerUtility::findTextGroupHandle(getParent());
		setTextGroup(textGroup);
	}

	PassiveLabel::~PassiveLabel() noexcept
	{
	}

	void PassiveLabel::onAnscestorChange(Container* _) noexcept
	{
		RenderableContainer::onAnscestorChange(_);
		if(m_isReassociateOnParentChange)
			tryReassociate();
	}

	void PassiveLabel::tryReassociate() noexcept
	{
		GfxDriverObjectHandleType textGroup = ContainerUtility::findTextGroupHandle(getParent());
		setTextGroup(textGroup);
	}

	void PassiveLabel::setReassociateOnParentChange(com::Bool isReassociate, com::Bool isApplyNow) noexcept
	{
		m_isReassociateOnParentChange = isReassociate;
		if(m_isReassociateOnParentChange && isApplyNow)
			tryReassociate();
	}
}