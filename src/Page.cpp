
#include "Common.h"
#include "Manager.h"
#include "Page.h"

//==============================================================================
// Page
//==============================================================================

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Page::Initialize(Manager* manager, CategoryItem* ownerCategory, const PathName& srcFileFullPath)
{
	m_manager = manager;
	m_ownerCategory = ownerCategory;
	m_srcFileRelPath = m_manager->GetSourceDirectory().MakeRelative(srcFileFullPath);
	m_srcFileFullPath = srcFileFullPath;
	m_outputFileRelPath = m_srcFileRelPath.ChangeExtension(_T("html"));
	m_outputFileFullPath = PathName(m_manager->GetReleaseDirectory(), m_outputFileRelPath);
	m_relpathToRoot = m_outputFileFullPath.GetParent().MakeRelative(m_manager->GetReleaseDirectory());

	m_manager->AddPage(this);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Page::MakePageContents()
{
	String text;
	String args = String::Format(_T("-f markdown -t html5 -o tmp \"{0}\""), m_srcFileFullPath);
	Process::Execute(_T("pandoc"), args);	// TODO: 標準出力のエンコーディングを指定したい・・・。とりあえず今は一度ファイルに落とす

	// 一時ファイルを開きなおして解析する
	StreamReader reader(_T("tmp"), Encoding::GetUTF8Encoding());
	StringWriter writer;
	String line;
	while (reader.ReadLine(&line))
	{
		// <h1> はキャプション
		MatchResult m1;
		if (Regex::Match(line, _T("<h1 id=\"(.*)\">(.*)</h1>"), &m1))
		{
			m_caption = m1[2];
		}
		// <h2> を集めておく
		MatchResult m;
		if (Regex::Match(line, _T("<h2 id=\"(.*)\">(.*)</h2>"), &m))
		{
			m_linkElementList.Add(LinkElement{ m[1], m[2] });
		}

		writer.WriteLine(line);
	}
	m_pageContentsText = writer.ToString();

	// ナビゲーション
	{
		// http://am-yu.net/2014/04/20/bootstrap3-affix-scrollspy/
		StringWriter writer;
		writer.WriteLine(_T(R"(<nav class="affix-nav"><ul class="nav">)"));
		writer.WriteLine(_T(R"(<li class="cap">page contents</li>)"));
		for (auto& e : m_linkElementList)
		{
			writer.WriteLine(String::Format(_T(R"(<li><a href="#{0}">{1}</a></li>)"), e.href, e.text));
		}
		writer.WriteLine(_T(R"(</ul></nav>)"));
		m_pageNavListText = writer.ToString();
	}
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Page::ExportPageFile()
{
	String tocTree;
	//if (m_ownerCategory->m_toc != nullptr) {
	//	tocTree = m_ownerCategory->m_toc->MakeTocTree(this);
	//}

	String pageText = m_manager->GetPageTemplateText();
	pageText = pageText.Replace(_T("$(LN_TO_ROOT_PATH)"), m_relpathToRoot);
	//pageText = pageText.Replace(_T("NAVBAR_ITEMS"), m_ownerCategory->MakeNavbarTextInActive(this));
	//pageText = pageText.Replace(_T("TOC_TREE"), tocTree);
	//pageText = pageText.Replace(_T("PAGE_CONTENTS"), m_pageContentsText);
	//pageText = pageText.Replace(_T("PAGE_INDEX_LIST"), m_pageNavListText);

	FileSystem::CreateDirectory(m_outputFileFullPath.GetParent());
	FileSystem::WriteAllText(m_outputFileFullPath.c_str(), pageText, Encoding::GetUTF8Encoding());
}
