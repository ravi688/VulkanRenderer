#include <sutk/VBoxContainer.hpp>

#include <common/assert.h>

namespace SUTK
{
	VBoxContainer::VBoxContainer(UIDriver& driver, Container* parent) noexcept : Container(driver, parent)
	{

	}
	void VBoxContainer::onAddChild(Container* child)
	{
		if(!child->isLayoutIgnore())
			recalculateLayout();
	}
	void VBoxContainer::onRemoveChild(Container* child)
	{
		if(!child->isLayoutIgnore())
			recalculateLayout();
	}
	void VBoxContainer::onResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged)
	{
		// call this first to pass updated rect value to Contaienr::onResize()
		recalculateLayout();
		// mandatory to be called
		Container::onResize(newRect, isPositionChanged, isSizeChanged);
	}
	void VBoxContainer::recalculateLayout()
	{
		/*
			Solution:

			min_height = 0
			pref_height = 0
			for each child in childs:
				min_height += child.min_height
				pref_height += child.pref_height
			if min_height >= rect.height:
				rect.height = min_height
			diff = rect.height - min_height
			req_diff = pref_height - min_height
			factor = diff / req_diff

			xpos = 0
			for each child in childs:
				child.width = rect.width
				height = min(child.min_height + factor * (child.pref_width - child.min_height), child.pref_height)
				child.height = height
				xpos += height

			Proof:
				factor * { sum over child.pref_width - sum over child.min_height } <= rect.height - min_height = diff
				factor * req_diff <= diff
		*/

		// calculate aggregate minimum height and prefered height
		std::vector<Container*> childs = getChilds();
		f32 minHeight = 0;
		f32 prefHeight = 0;
		for(Container* &child : childs)
		{
			// if this child doesn't care about Layout rules, then skip it.
			if(child->isLayoutIgnore())
				continue;

			LayoutAttributes& attrs = child->getLayoutAttributes();
			minHeight += attrs.minSize.height;
			prefHeight += attrs.prefSize.height;
		}

		// if this container is not long enough to accomodate minimimum height requirements for all of the child containers
		// then extend its height to match the exact required "minimum" height.
		if(minHeight >= getRect().height)
		{
			// WARNING: we can't use 'setSize' here, because it calls Container::onResize internally,
			// and since we are also calling recalculateLayout inside Container::onResize, it might lead to
			// infinite recursion.
			// Do not use --> setSize({ getSize().width, minHeight });
			getRawRectRef().setSize({ getSize().width, minHeight });
		}

		// available extra space after satisfying the aggregate minimum height requirement
		f32 diff = getRect().height - minHeight;
		// required extra space to attain aggregate preferred height on top of the minimum height requirement
		f32 reqDiff = prefHeight - minHeight;
		// the factor by which reqDiff can be decreased to fit into the available space 'diff', though aggregate preferred height won't be satisfied
		f32 factor = (reqDiff == 0.0f) ? 1.0f : (diff / reqDiff);

		// now layout the child containers
		f32 xpos = 0;
		for(Container* &child : childs)
		{
			if(child->isLayoutIgnore())
				continue;

			LayoutAttributes& attrs = child->getLayoutAttributes();
			f32 height = std::min(attrs.minSize.height + factor * (attrs.prefSize.height - attrs.minSize.height), attrs.prefSize.height);
			child->setRect({ 0, xpos, getRect().width, height });
			xpos += height;
		}
	}
}