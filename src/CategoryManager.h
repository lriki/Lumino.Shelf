#pragma once
#include "CategoryItem.h"

class CategoryManager
{
public:
	void Initialize(Manager* manager, const PathName& categoriesFile);

	Manager* GetManager() const { return m_manager; }

	String MakeNavBarListText(Page* curPage) const;

private:
	void MakeNavBarItemText(StringWriter* writer, Page* curPage, CategoryItem* item, bool active) const;

	Manager*	m_manager;
	Array<CategoryItemPtr>	m_rootCategoryList;
};
