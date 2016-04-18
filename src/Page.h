#pragma once

class Page
	: public Object
{
public:
	Page(Manager* manager, CategoryItem* ownerCategory);
	void PostSerialize();
	void AddChild(Page* child);
	Array<PagePtr>& GetChildren() { return m_children; }

	const String& GetCaption() const { return m_caption; }

	CategoryItem* GetOwnerCategory() const { return m_ownerCategory; }
	const PathName& GetOutputRelPath() const { return m_outputFileRelPath; }
	const PathName& GetRelPathToRoot() const { return m_relpathToRoot; }

	String MakeRelativePath(Page* page) const;	// このページから、root に戻って、指定のページに移動するようなパス
	void MakePageContents();
	void ExportPageFile();

private:
	struct LinkElement
	{
		String	href;
		String	text;
	};

	friend class Serializer;
	Manager*			m_manager;
	CategoryItem*		m_ownerCategory;
	PathName			m_srcFullPath;		// set from Serializer
	PathName			m_templateFullPath;	// set from Serializer
	String				m_caption;			// set from Serializer

	PathName			m_srcFileRelPath;
	PathName			m_srcFileFullPath;
	PathName			m_outputFileRelPath;
	PathName			m_outputFileFullPath;
	PathName			m_relpathToRoot;

	Page*				m_parentPage;
	Array<PagePtr>		m_children;

	Array<LinkElement>	m_linkElementList;
	String				m_pageContentsText;
	String				m_pageSideNavText;

#if 0
public:
	void Initialize(Manager* manager, CategoryItem* ownerCategory, const PathName& srcFileFullPath, Page* parent);

	String MakeRelativePath(Page* page) const;	// このページから、root に戻って、指定のページに移動するようなパス
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

	friend class Serializer;
	Manager*			m_manager;
	CategoryItem*		m_ownerCategory;
	PathName			m_srcFullPath;		// set from Serializer
	PathName			m_templateFullPath;	// set from Serializer


	PathName			m_srcFileRelPath;
	PathName			m_srcFileFullPath;
	PathName			m_outputFileRelPath;
	PathName			m_outputFileFullPath;
	PathName			m_relpathToRoot;

	Page*				m_parentPage;
	Array<PagePtr>		m_children;

	String				m_caption;
	Array<LinkElement>	m_linkElementList;
	String				m_pageContentsText;
	String				m_pageSideNavText;
#endif
};
