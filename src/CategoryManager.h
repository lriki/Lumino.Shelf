#pragma once
#include "CategoryItem.h"

class CategoryManager
{
public:
	void Initialize(Manager* manager, const PathName& categoriesFile);

	Manager* GetManager() const { return m_manager; }

private:
	Manager*	m_manager;
	Array<CategoryItemPtr>	m_rootCategoryList;
};
