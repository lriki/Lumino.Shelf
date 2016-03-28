#pragma once
#include "CategoryManager.h"

class Manager
{
public:
	void SetSourceDirectory(const PathName& path) { m_sourceDirectory = path; }
	const PathName& GetSourceDirectory() const { return m_sourceDirectory; }

	void SetTemplateDirectory(const PathName& path) { m_templateDirectory = path; }
	const PathName& GetTemplateDirectory() const { return m_templateDirectory; }

	void SetReleaseDirectory(const PathName& path) { m_releaseDirectory = path; }
	const PathName& GetReleaseDirectory() const { return m_releaseDirectory; }

	void Build();

	String GetPageTemplateText();
	void AddPage(PagePtr page) { m_allPages.Add(page); }

private:
	PathName		m_sourceDirectory;
	PathName		m_templateDirectory;
	PathName		m_releaseDirectory;
	CategoryManager	m_categoryManager;
	PagePtr			m_homePage;
	Array<PagePtr>	m_allPages;
};
