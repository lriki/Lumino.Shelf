
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
#if 0
	PathName homePageFile(m_sourceDirectory, _T("index.md"));
	m_homePage = PagePtr::MakeRef();
	m_homePage->Initialize(this, nullptr, homePageFile, nullptr);

	PathName categoriesFile(m_sourceDirectory, _T("categories.md"));
	m_categoryManager.Initialize(this, categoriesFile);

#endif

	// ページ内容を作る
	for (PagePtr page : m_allPages)
	{
		page->MakePageContents();
	}

	// 出力する
	for (PagePtr page : m_allPages)
	{
		page->ExportPageFile();
	}

	// template フォルダからそのままコピーするもの
	FileSystem::CopyDirectory(PathName(m_templateDirectory, _T("bootstrap")), PathName(m_releaseDirectory, _T("bootstrap")), true, true);
	FileSystem::Copy(PathName(m_templateDirectory, _T("github-markdown.css")), PathName(m_releaseDirectory, _T("github-markdown.css")), true);
}


//------------------------------------------------------------------------------
//
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
