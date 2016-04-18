#pragma once
#include "Page.h"
#include "PageToc.h"

class CategoryItem
	: public Object
{
public:
	CategoryItem(CategoryManager* manager);
	void PostSerialize();

	CategoryManager* GetCategoryManager() const { return m_manager; }
	void AddChild(CategoryItem* child);
	const Array<CategoryItemPtr>& GetChildren() const { return m_children; }

	String GetCaption() const;
	Page* GetHomePage() const { return m_homePage; }
	PageToc* GetToc() const { return m_toc; }

private:
	friend class Serializer;
	CategoryManager*		m_manager;
	PathName				m_srcPagePath;	// set from Serializer
	String					m_caption;		// set from Serializer
	PageTocPtr				m_toc;			// set from Serializer

	CategoryItem*			m_parent;
	Array<CategoryItemPtr>	m_children;

	PagePtr					m_homePage;

#if 0
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
#endif
};
