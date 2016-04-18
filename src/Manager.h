#pragma once
#include "CategoryManager.h"

class Manager;
using ManagerPtr = RefPtr<Manager>;

class Manager
	: public Object
{
public:
	void SetSourceDirectory(const PathName& path) { m_sourceDirectory = path; }
	const PathName& GetSourceDirectory() const { return m_sourceDirectory; }

	void SetTemplateDirectory(const PathName& path) { m_templateDirectory = path; }
	const PathName& GetTemplateDirectory() const { return m_templateDirectory; }

	void SetReleaseDirectory(const PathName& path) { m_releaseDirectory = path; }
	const PathName& GetReleaseDirectory() const { return m_releaseDirectory; }


	CategoryManager* GetCategoryManager() { return m_categoryManager; }
	void Build();

	String GetPageTemplateText();
	void AddPage(PagePtr page) { m_allPages.Add(page); }

private:
	friend class Serializer;
	PathName			m_sourceDirectory;
	PathName			m_templateDirectory;
	PathName			m_releaseDirectory;
	CategoryManagerPtr	m_categoryManager;
	PagePtr				m_homePage;


	Array<PagePtr>		m_allPages;
};
