
#include "Common.h"
#include "Manager.h"

//==============================================================================
// Manager
//==============================================================================

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Manager::Build()
{
	PathName homePageFile(m_sourceDirectory, _T("index.md"));
	m_homePage = PagePtr::MakeRef();
	m_homePage->Initialize(this, nullptr, homePageFile);

	PathName categoriesFile(m_sourceDirectory, _T("categories.md"));
	m_categoryManager.Initialize(this, categoriesFile);


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
}


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
String Manager::GetPageTemplateText()
{
	return FileSystem::ReadAllText(PathName(m_templateDirectory, _T("page.html")).c_str(), Encoding::GetUTF8Encoding());
}
