
#pragma once
#include "Manager.h"
#include "CategoryManager.h"
#include "PageToc.h"

class Serializer
{
public:
	ManagerPtr LoadManager(const PathName& xmlFileFullPath);
	void LoadCategoryIndex(CategoryItem* category, const PathName& xmlFileFullPath);

private:
	ManagerPtr Load_shelf(XmlFileReader* reader);
	PagePtr Load_homepage(XmlFileReader* reader, CategoryItem* ownerCategory);
	PagePtr Load_page(XmlFileReader* reader, Page* parent, CategoryItem* ownerCategory);
	CategoryManagerPtr Load_categories(XmlFileReader* reader);
	CategoryItemPtr Load_category(XmlFileReader* reader, CategoryManager* manager, CategoryItem* parent);

	void Load_category_index(XmlFileReader* reader, CategoryItem* category);
	PageTocPtr Load_toc(XmlFileReader* reader, CategoryItem* ownerCategory);

	Manager*			m_manager;
};
