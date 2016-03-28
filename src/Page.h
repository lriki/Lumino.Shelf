#pragma once

class Page
	: public Object
{
public:
	void Initialize(Manager* manager, CategoryItem* ownerCategory, const PathName& srcFileFullPath);
	void MakePageContents();
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

	String				m_caption;
	Array<LinkElement>	m_linkElementList;
	String				m_pageContentsText;
	String				m_pageNavListText;
};
