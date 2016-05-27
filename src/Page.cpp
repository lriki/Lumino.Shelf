
#include "Common.h"
#include "Manager.h"
#include "Page.h"

//==============================================================================
// Page
//==============================================================================

//------------------------------------------------------------------------------
Page::Page(Manager* manager, CategoryItem* ownerCategory)
	: m_manager(manager)
	, m_ownerCategory(ownerCategory)
	, m_parentPage(nullptr)
{
}

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
void Page::AddChild(Page* child)
{
	m_children.Add(child);
	child->m_parentPage = this;
}

//-----------------------------------------------------------------------------
String Page::MakeRelativePath(Page* page) const
{
	String path = String::Format(_T("{0}/{1}"), GetRelPathToRoot(), page->GetOutputRelPath());
	return path.Replace(_T("\\"), _T("/"));
}

//-----------------------------------------------------------------------------
void Page::ResolveExtensions(TemporaryFile* file) const
{
	int tabSectionCount = 0;
	{
		StreamReader reader(m_srcFileFullPath);
		String line;
		while (reader.ReadLine(&line))
		{
			if (line.IndexOf(_T("<sync-tabs>")) >= 0)
			{
				++tabSectionCount;
			}
		}
	}

	String cppTabId, cTabId, csTabId, rubyTabId, hspTabId;
	for (int i = 0; i < tabSectionCount; ++i)
	{
		if (!cppTabId.IsEmpty()) cppTabId += _T(",");
		cppTabId += String::Format(_T("#cpp{0}"), i + 1);
		if (!cTabId.IsEmpty()) cTabId += _T(",");
		cTabId += String::Format(_T("#c{0}"), i + 1);
		if (!csTabId.IsEmpty()) csTabId += _T(",");
		csTabId += String::Format(_T("#cs{0}"), i + 1);
		if (!rubyTabId.IsEmpty()) rubyTabId += _T(",");
		rubyTabId += String::Format(_T("#ruby{0}"), i + 1);
		if (!hspTabId.IsEmpty()) hspTabId += _T(",");
		hspTabId += String::Format(_T("#hsp{0}"), i + 1);
	}



	int paneCount = 0;

	StreamReader reader(m_srcFileFullPath);
	StreamWriter writer(file);
	String line;
	while (reader.ReadLine(&line))
	{
		MatchResult m;
		if (line.IndexOf(_T("<sync-tabs>")) >= 0)
		{
			writer.WriteLine(_T(R"(<ul id="ln_sync_tabs" class="nav nav-tabs">)"));
			writer.WriteLine(_T(R"(<li class="active"><a href="#cpp-tab" data-target="{0}" data-toggle="tab">C++</a></li>)"), cppTabId);
			writer.WriteLine(_T(R"(<li><a href="#c-tab" data-target="{0}" data-toggle="tab">C</a></li>)"), cTabId);
			writer.WriteLine(_T(R"(<li><a href="#cs-tab" data-target="{0}" data-toggle="tab">C#</a></li>)"), csTabId);
			writer.WriteLine(_T(R"(<li><a href="#ruby-tab" data-target="{0}" data-toggle="tab">Ruby</a></li>)"), rubyTabId);
			writer.WriteLine(_T(R"(<li><a href="#hsp-tab" data-target="{0}" data-toggle="tab">HSP</a></li>)"), hspTabId);
			writer.WriteLine(_T(R"(</ul>)"));
			writer.WriteLine(_T(R"(<div id="ln_sync_tab_content" class="tab-content">)"));
		}
		else if (Regex::Search(line, _T("<sync-tab-pane group=\"(.*)\">"), &m))
		{
			if (paneCount == 0)
				writer.WriteLine(_T(R"(<div class="tab-pane fade in active" id="{0}{1}">)"), m[1], paneCount + 1);
			else
				writer.WriteLine(_T(R"(<div class="tab-pane fade" id="{0}{1}">)"), m[1], paneCount + 1);
			++paneCount;
		}
		else if (line.IndexOf(_T("</sync-tab-pane>")) >= 0)
		{
			writer.WriteLine(_T("</div>"));
		}
		else if (line.IndexOf(_T("</sync-tabs>")) >= 0)
		{
			writer.WriteLine(_T("</div>"));
			paneCount = 0;
		}
		else
		{
			writer.WriteLine(line);
		}
	}
 }

//------------------------------------------------------------------------------
void Page::MakePageContents()
{
	if (IsRawHTMLPage())
	{
		m_pageContentsText = FileSystem::ReadAllText(m_srcFileFullPath, Encoding::GetUTF8Encoding());

		StreamReader reader(m_srcFileFullPath, Encoding::GetUTF8Encoding());
		String line;
		while (reader.ReadLine(&line))
		{
			MatchResult m;
			if (Regex::Search(line, _T("<h1>(.*)</h1>"), &m))
			{
				m_caption = m[1];
				break;
			}
		}
	}
	else
	{
		TemporaryFile tmpFile;
		tmpFile.Open();
		ResolveExtensions(&tmpFile);
		tmpFile.Close();

		String text;
		String args = String::Format(_T("-f markdown -t html5 -o tmp \"{0}\""), tmpFile.GetFilePath()/*m_srcFileFullPath*/);
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
			//writer.WriteLine(_T(R"(<div class="col-xs-2">)"));
			writer.WriteLine(_T(R"(<div id="scroll-spy-target" class="affix-nav" data-spy="affix"><ul class="nav">)"));
			writer.WriteLine(_T(R"(<li class="cap">page contents</li>)"));
			for (auto& e : m_linkElementList)
			{
				writer.WriteLine(String::Format(_T(R"(<li><a href="#{0}">{1}</a></li>)"), e.href, e.text));
			}
			writer.WriteLine(_T(R"(</ul></div>)"));
			//writer.WriteLine(_T(R"(</div>)"));
			m_pageSideNavText = writer.ToString();
		}
	}
}

//------------------------------------------------------------------------------
void Page::ExportPageFile()
{
	if (IsRawHTMLPage())
	{
		String pageText = m_manager->GetPageTemplateText(m_templateFullPath);
		pageText = pageText.Replace(_T("$(LN_TO_ROOT_PATH)"), m_relpathToRoot);
		pageText = pageText.Replace(_T("$(PAGE_TITLE)"), m_caption);
		pageText = pageText.Replace(_T("$(SITE_TITLE)"), m_manager->GetSiteTitle());
		pageText = pageText.Replace(_T("$(NAVBAR_ITEMS)"), m_manager->GetCategoryManager()->MakeNavBarListText(this));
		pageText = pageText.Replace(_T("$(PAGE_CONTENTS)"), m_pageContentsText);
		pageText = pageText.Replace(_T("$(PAGE_FOOTER)"), m_manager->GetFooterText());

		FileSystem::CreateDirectory(m_outputFileFullPath.GetParent());
		FileSystem::WriteAllText(m_outputFileFullPath.c_str(), pageText, Encoding::GetUTF8Encoding());
	}
	else
	{
		String tocTree;
		if (m_ownerCategory != nullptr && m_ownerCategory->GetToc() != nullptr) {
			tocTree = m_ownerCategory->GetToc()->MakeTocTreeText(this);
		}

		int contentsCols = 7;

		StringWriter contentsText;
		//contentsText.WriteLine(_T("<div class=\"col-md-{0}\">"), contentsCols);
		contentsText.WriteLine(_T("<arctile class=\"markdown-body\">"));
		contentsText.WriteLine(m_pageContentsText);
		contentsText.WriteLine(_T("</arctile>"));
		//contentsText.WriteLine(_T("</div>"));

		String pageText = m_manager->GetPageTemplateText(m_templateFullPath);
		pageText = pageText.Replace(_T("$(LN_TO_ROOT_PATH)"), m_relpathToRoot);
		pageText = pageText.Replace(_T("$(PAGE_TITLE)"), m_caption);
		pageText = pageText.Replace(_T("$(SITE_TITLE)"), m_manager->GetSiteTitle());
		pageText = pageText.Replace(_T("$(NAVBAR_ITEMS)"), m_manager->GetCategoryManager()->MakeNavBarListText(this));
		pageText = pageText.Replace(_T("$(PAGE_TOC)"), tocTree);
		pageText = pageText.Replace(_T("$(PAGE_CONTENTS)"), contentsText.ToString());
		pageText = pageText.Replace(_T("$(PAGE_SIDENAV)"), m_pageSideNavText);
		pageText = pageText.Replace(_T("$(PAGE_FOOTER)"), m_manager->GetFooterText());

		FileSystem::CreateDirectory(m_outputFileFullPath.GetParent());
		FileSystem::WriteAllText(m_outputFileFullPath.c_str(), pageText, Encoding::GetUTF8Encoding());
	}
}

