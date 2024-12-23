#pragma once

#include <sutk/Label.hpp>

namespace SUTK
{
	class TextGroupContainer;
	
	// A PassiveLabel is slightly different from the typical Label in the sense that upon instantiation it first searches up the Container Hierarhcy (recursively)
	// for a Text Group object, if it finds then it uses that Text Group object to create a SmallText instance internally.
	// If it doesn't find then it creates a Text Group object internally and then creates a SmallText instance from this Text Group object.
	// When this PassiveLabel object is destroyed then it also destroys the (if any) created Text Group object.
	class SUTK_API PassiveLabel : public Label
	{
	private:
		com::Bool m_isReassociateOnParentChange;
		void tryReassociate() noexcept;
	protected:
		// Override of Label::onAnscestorChange()
		virtual void onAnscestorChange(Container* anscestor) noexcept override;
	public:
		// If no textGroup is provided, then it creates a new text group internally only for this PassiveLabel
		// PERF: It is recommened to specify a valid text group if possible to avoid creating separate text group for each label which is inefficient
		PassiveLabel(UIDriver& driver, Container* parent = nullptr, GfxDriverObjectHandleType textGroup = GFX_DRIVER_OBJECT_NULL_HANDLE) noexcept;
		~PassiveLabel() noexcept;

		void setReassociateOnParentChange(com::Bool isReassociate, com::Bool isApplyNow = com::False) noexcept;
	};
}