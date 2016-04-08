#pragma once
#include "Page.h"

class PageTocItem;
typedef RefPtr<PageTocItem> PageTocItemPtr;

class PageTocItem
	: public Object
{
public:
	void Initialize(PageToc* toc, const String& src);

	void SetParent(PageTocItem* parent) { m_parent = parent; }
	PageTocItem* GetParent() const { return m_parent; }

	void AddChild(PageTocItem* item) { m_childItems.Add(item); item->SetParent(this); }
	const Array<PageTocItemPtr>& GetChildren() const { return m_childItems; }

	String GetCaption() const;
	Page* GetPage() const { return m_page; }

private:
	PageToc*				m_ownerToc;
	PageTocItem*			m_parent;
	Array<PageTocItemPtr>	m_childItems;

	String					m_caption;
	PagePtr					m_page;
};
