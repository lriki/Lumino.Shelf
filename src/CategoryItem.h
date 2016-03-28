#pragma once
#include "Page.h"

class CategoryItem
	: public Object
{
public:
	void Initialize(CategoryManager* manager, CategoryItem* parent, const String& source);

private:
	CategoryManager*	m_manager;
	CategoryItem*		m_parent;
	CategoryItemSource	m_itemSource;
	String				m_sourceString;

	PagePtr				m_indexPage;	// m_itemSource ‚ª IndexFile ‚Ì‚Æ‚«‚Ìƒtƒ@ƒCƒ‹
};
