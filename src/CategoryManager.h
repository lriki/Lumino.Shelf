#pragma once
#include "CategoryItem.h"

class CategoryManager;
using CategoryManagerPtr = RefPtr<CategoryManager>;

class CategoryManager
	: public Object
{
public:
	CategoryManager(Manager* manager);

	//void Initialize(Manager* manager, const PathName& categoriesFile);

	Manager* GetManager() const { return m_manager; }

	String MakeNavBarListText(Page* curPage) const;

private:
	void MakeNavBarItemText(StringWriter* writer, Page* curPage, CategoryItem* item, bool active) const;

	friend class Serializer;
	Manager*	m_manager;
	Array<CategoryItemPtr>	m_rootCategoryList;
};
