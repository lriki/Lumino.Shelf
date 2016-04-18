
#include "Common.h"
#include "Manager.h"
#include "Page.h"

//==============================================================================
// Page
//==============================================================================

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
Page::Page(Manager* manager, CategoryItem* ownerCategory)
	: m_manager(manager)
	, m_ownerCategory(ownerCategory)
	, m_parentPage(nullptr)
{
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Page::PostSerialize()
{
	if (!m_srcFullPath.IsEmpty())
	{
		m_srcFileRelPath = m_manager->GetSourceDirectory().MakeRelative(m_srcFullPath);
		m_srcFileFullPath = m_srcFullPath;
		m_outputFileRelPath = m_srcFileRelPath.ChangeExtension(_T("html"));
		m_outputFileFullPath = PathName(m_manager->GetReleaseDirectory(), m_outputFileRelPath);
		m_relpathToRoot = m_outputFileFullPath.GetParent().MakeRelative(m_manager->GetReleaseDirectory());
		m_manager->AddPage(this);
	}
	else
	{
		// caption だけ持っているようなダミーノードとして使うページ
	}
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Page::AddChild(Page* child)
{
	m_children.Add(child);
	child->m_parentPage = this;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
String Page::MakeRelativePath(Page* page) const
{
	return String::Format(_T("{0}/{1}"), GetRelPathToRoot(), page->GetOutputRelPath());
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Page::MakePageContents()
{
	String text;
	String args = String::Format(_T("-f markdown -t html5 -o tmp \"{0}\""), m_srcFileFullPath);
	String stdErr;
	int exitCode = Process::Execute(_T("pandoc"), args, nullptr, &stdErr);	// TODO: 標準出力のエンコーディングを指定したい・・・。とりあえず今は一度ファイルに落とす
	if (exitCode != 0)
	{
		Console::WriteLineError(stdErr);
		LN_THROW(0, InvalidOperationException);
	}

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
		writer.WriteLine(_T(R"(<div class="col-xs-2">)"));
		writer.WriteLine(_T(R"(<nav class="affix-nav"><ul class="nav">)"));
		writer.WriteLine(_T(R"(<li class="cap">page contents</li>)"));
		for (auto& e : m_linkElementList)
		{
			writer.WriteLine(String::Format(_T(R"(<li><a href="#{0}">{1}</a></li>)"), e.href, e.text));
		}
		writer.WriteLine(_T(R"(</ul></nav>)"));
		writer.WriteLine(_T(R"(</div>)"));
		m_pageSideNavText = writer.ToString();
	}
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Page::ExportPageFile()
{
	String tocTree;
	if (m_ownerCategory != nullptr && m_ownerCategory->GetToc() != nullptr) {
		tocTree = m_ownerCategory->GetToc()->MakeTocTreeText(this);
	}

	int contentsCols = 8;

	StringWriter contentsText;
	contentsText.WriteLine(_T("<div class=\"col-md-{0}\">"), contentsCols);
	contentsText.WriteLine(_T("<arctile class=\"markdown-body\">"));
	contentsText.WriteLine(m_pageContentsText);
	contentsText.WriteLine(_T("</arctile>"));
	contentsText.WriteLine(_T("</div>"));

	String pageText = m_manager->GetPageTemplateText();
	pageText = pageText.Replace(_T("$(LN_TO_ROOT_PATH)"), m_relpathToRoot);
	pageText = pageText.Replace(_T("$(NAVBAR_ITEMS)"), m_manager->GetCategoryManager()->MakeNavBarListText(this));
	pageText = pageText.Replace(_T("$(PAGE_TOC)"), tocTree);
	pageText = pageText.Replace(_T("$(PAGE_CONTENTS)"), contentsText.ToString());
	pageText = pageText.Replace(_T("$(PAGE_SIDENAV)"), m_pageSideNavText);

	FileSystem::CreateDirectory(m_outputFileFullPath.GetParent());
	FileSystem::WriteAllText(m_outputFileFullPath.c_str(), pageText, Encoding::GetUTF8Encoding());
}

#if 0
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Page::Initialize(Manager* manager, CategoryItem* ownerCategory, const PathName& srcFileFullPath, Page* parent)
{
	m_manager = manager;
	m_ownerCategory = ownerCategory;
	m_srcFileRelPath = m_manager->GetSourceDirectory().MakeRelative(srcFileFullPath);
	m_srcFileFullPath = srcFileFullPath;
	m_outputFileRelPath = m_srcFileRelPath.ChangeExtension(_T("html"));
	m_outputFileFullPath = PathName(m_manager->GetReleaseDirectory(), m_outputFileRelPath);
	m_relpathToRoot = m_outputFileFullPath.GetParent().MakeRelative(m_manager->GetReleaseDirectory());
	m_parentPage = parent;
	m_manager->AddPage(this);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
String Page::MakeRelativePath(Page* page) const
{
	return String::Format(_T("{0}/{1}"), GetRelPathToRoot(), page->GetOutputRelPath());
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Page::MakePageContents()
{
	String text;
	String args = String::Format(_T("-f markdown -t html5 -o tmp \"{0}\""), m_srcFileFullPath);
	String stdErr;
	int exitCode = Process::Execute(_T("pandoc"), args, nullptr, &stdErr);	// TODO: 標準出力のエンコーディングを指定したい・・・。とりあえず今は一度ファイルに落とす
	if (exitCode != 0)
	{
		Console::WriteLineError(stdErr);
		LN_THROW(0, InvalidOperationException);
	}

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
		writer.WriteLine(_T(R"(<div class="col-xs-2">)"));
		writer.WriteLine(_T(R"(<nav class="affix-nav"><ul class="nav">)"));
		writer.WriteLine(_T(R"(<li class="cap">page contents</li>)"));
		for (auto& e : m_linkElementList)
		{
			writer.WriteLine(String::Format(_T(R"(<li><a href="#{0}">{1}</a></li>)"), e.href, e.text));
		}
		writer.WriteLine(_T(R"(</ul></nav>)"));
		writer.WriteLine(_T(R"(</div>)"));
		m_pageSideNavText = writer.ToString();
	}
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Page::ExportPageFile()
{
	String tocTree;
	if (m_ownerCategory != nullptr && m_ownerCategory->GetToc() != nullptr) {
		tocTree = m_ownerCategory->GetToc()->MakeTocTreeText(this);
	}

	int contentsCols = 8;

	StringWriter contentsText;
	contentsText.WriteLine(_T("<div class=\"col-md-{0}\">"), contentsCols);
	contentsText.WriteLine(_T("<arctile class=\"markdown-body\">"));
	contentsText.WriteLine(m_pageContentsText);
	contentsText.WriteLine(_T("</arctile>"));
	contentsText.WriteLine(_T("</div>"));
	
	String pageText = m_manager->GetPageTemplateText();
	pageText = pageText.Replace(_T("$(LN_TO_ROOT_PATH)"), m_relpathToRoot);
	pageText = pageText.Replace(_T("$(NAVBAR_ITEMS)"), m_manager->GetCategoryManager()->MakeNavBarListText(this));
	pageText = pageText.Replace(_T("$(PAGE_TOC)"), tocTree);
	pageText = pageText.Replace(_T("$(PAGE_CONTENTS)"), contentsText.ToString());
	pageText = pageText.Replace(_T("$(PAGE_SIDENAV)"), m_pageSideNavText);

	FileSystem::CreateDirectory(m_outputFileFullPath.GetParent());
	FileSystem::WriteAllText(m_outputFileFullPath.c_str(), pageText, Encoding::GetUTF8Encoding());
}

#endif
