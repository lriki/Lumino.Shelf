#pragma once
#include "Page.h"
#include "PageToc.h"

class CategoryItem
	: public Object
{
public:
	void Initialize(CategoryManager* manager, CategoryItem* parent, const String& source);
	CategoryManager* GetCategoryManager() const { return m_manager; }
	String GetCaption() const;
	Page* GetIndexPage() const { return m_indexPage; }
	PageToc* GetToc() const { return m_toc; }

	void AddChild(CategoryItem* item) { m_childList.Add(item); }
	const Array<CategoryItemPtr>& GetChildren() const { return m_childList; }

private:
	
	CategoryManager*		m_manager;
	CategoryItem*			m_parent;
	Array<CategoryItemPtr>	m_childList;
	CategoryItemSource		m_itemSource;
	String					m_sourceString;

	PagePtr					m_indexPage;
	PageTocPtr				m_toc;
};
