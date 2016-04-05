#pragma once

class Page
	: public Object
{
public:
	void Initialize(Manager* manager, CategoryItem* ownerCategory, const PathName& srcFileFullPath);

	CategoryItem* GetOwnerCategory() const { return m_ownerCategory; }
	const PathName& GetOutputRelPath() const { return m_outputFileRelPath; }
	const PathName& GetRelPathToRoot() const { return m_relpathToRoot; }

	Array<PagePtr>& GetChildren() { return m_children; }

	void MakePageContents();
	const String& GetCaption() const { return m_caption; }

	void ExportPageFile();

private:
	struct LinkElement
	{
		String	href;
		String	text;
	};

	Manager*			m_manager;
	CategoryItem*		m_ownerCategory;
	PathName			m_srcFileRelPath;
	PathName			m_srcFileFullPath;
	PathName			m_outputFileRelPath;
	PathName			m_outputFileFullPath;
	PathName			m_relpathToRoot;

	Array<PagePtr>		m_children;

	String				m_caption;
	Array<LinkElement>	m_linkElementList;
	String				m_pageContentsText;
	String				m_pageSideNavText;
};
