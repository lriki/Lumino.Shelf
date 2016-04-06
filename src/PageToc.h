#pragma once
#include "PageTocItem.h"

class PageToc;
typedef RefPtr<PageToc> PageTocPtr;

class PageToc
	: public Object
{
public:
	void Initialize(CategoryItem* owner, const PathName& tocFilePath);

	CategoryItem* GetOwnerCategory() const { return m_ownerCategory; }
	const PathName& GetDirectoryFullPath() const { return m_dirFullPath; }

private:
	CategoryItem*			m_ownerCategory;
	Array<PageTocItemPtr>	m_rootItems;
	PathName				m_dirFullPath;
};
