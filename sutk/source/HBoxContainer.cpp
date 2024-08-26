#include <sutk/HBoxContainer.hpp>

#include <common/assert.h>

namespace SUTK
{
	HBoxContainer::HBoxContainer(UIDriver& driver, Container* parent) noexcept : Container(driver, parent)
	{

	}
	void HBoxContainer::onAddChild(Container* child)
	{
		if(!child->isLayoutIgnore())
			recalculateLayout();
	}
	void HBoxContainer::onRemoveChild(Container* child)
	{
		if(!child->isLayoutIgnore())
			recalculateLayout();
	}
	void HBoxContainer::onResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged)
	{
		// call this first to pass updated rect value to Contaienr::onResize()
		recalculateLayout();
		// mandatory to be called
		Container::onResize(newRect, isPositionChanged, isSizeChanged);
	}
	void HBoxContainer::recalculateLayout()
	{
		/*
			Solution:

			min_width = 0
			pref_width = 0
			for each child in childs:
				min_width += child.min_width
				pref_width += child.pref_width
			if min_width >= rect.width:
				rect.width = min_width
			diff = rect.width - min_width
			req_diff = pref_width - min_width
			factor = diff / req_diff

			xpos = 0
			for each child in childs:
				child.width = rect.width
				width = min(child.min_width + factor * (child.pref_width - child.min_width), child.pref_width)
				child.width = width
				xpos += width

			Proof:
				factor * { sum over child.pref_width - sum over child.min_width } <= rect.width - min_width = diff
				factor * req_diff <= diff
		*/

		// calculate aggregate minimum width and prefered width
		std::vector<Container*> childs = getChilds();
		f32 minWidth = 0;
		f32 prefWidth = 0;
		for(Container* &child : childs)
		{
			// if this child doesn't care about Layout rules, then skip it.
			if(child->isLayoutIgnore())
				continue;

			LayoutAttributes& attrs = child->getLayoutAttributes();
			minWidth += attrs.minSize.width;
			prefWidth += attrs.prefSize.width;
		}

		// if this container is not long enough to accomodate minimimum width requirements for all of the child containers
		// then extend its width to match the exact required "minimum" width.
		if(minWidth >= getRect().width)
		{
			// WARNING: we can't use 'setSize' here, because it calls Container::onResize internally,
			// and since we are also calling recalculateLayout inside Container::onResize, it might lead to
			// infinite recursion.
			// Do not use --> setSize({ getSize().width, minWidth });
			getRawRectRef().setSize({ minWidth, getSize().height });
		}

		// available extra space after satisfying the aggregate minimum width requirement
		f32 diff = getRect().width - minWidth;
		// required extra space to attain aggregate preferred width on top of the minimum width requirement
		f32 reqDiff = prefWidth - minWidth;
		// the factor by which reqDiff can be decreased to fit into the available space 'diff', though aggregate preferred width won't be satisfied
		f32 factor = (reqDiff == 0.0f) ? 1.0f : (diff / reqDiff);

		// now layout the child containers
		f32 xpos = 0;
		for(Container* &child : childs)
		{
			if(child->isLayoutIgnore())
				continue;
			
			LayoutAttributes& attrs = child->getLayoutAttributes();
			f32 width = std::min(attrs.minSize.width + factor * (attrs.prefSize.width - attrs.minSize.width), attrs.prefSize.width);
			child->setRect({ xpos, 0, width, getRect().height });
			xpos += width;
		}
	}
}