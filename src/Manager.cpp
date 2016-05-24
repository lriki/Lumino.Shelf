
#include "Common.h"
#include "Manager.h"

//==============================================================================
// Manager
//==============================================================================

//------------------------------------------------------------------------------
void Manager::Build()
{
#if 0
	PathName homePageFile(m_sourceDirectory, _T("index.md"));
	m_homePage = PagePtr::MakeRef();
	m_homePage->Initialize(this, nullptr, homePageFile, nullptr);

	PathName categoriesFile(m_sourceDirectory, _T("categories.md"));
	m_categoryManager.Initialize(this, categoriesFile);

#endif

	// �y�[�W���e�����
	for (PagePtr page : m_allPages)
	{
		page->MakePageContents();
	}

	// �o�͂���
	for (PagePtr page : m_allPages)
	{
		page->ExportPageFile();
	}

	// template �t�H���_���炻�̂܂܃R�s�[�������
	FileSystem::CopyDirectory(PathName(m_templateDirectory, _T("bootstrap")), PathName(m_releaseDirectory, _T("bootstrap")), true, true);
	FileSystem::Copy(PathName(m_templateDirectory, _T("github-markdown.css")), PathName(m_releaseDirectory, _T("github-markdown.css")), true);

	// favicon
	FileSystem::Copy(PathName(m_sourceDirectory, m_faviconPath), PathName(m_releaseDirectory, m_faviconPath), true);
}

//------------------------------------------------------------------------------
String Manager::GetPageTemplateText(const PathName& priority)
{
	if (priority.IsEmpty())
	{
		return FileSystem::ReadAllText(PathName(m_templateDirectory, _T("page.html")).c_str(), Encoding::GetUTF8Encoding());
	}
	else
	{
		return FileSystem::ReadAllText(priority.c_str(), Encoding::GetUTF8Encoding());
	}
}

//------------------------------------------------------------------------------
String Manager::GetFooterText() const
{
	return FileSystem::ReadAllText(PathName(m_sourceDirectory, m_footerTemplatePath), Encoding::GetUTF8Encoding());
}
