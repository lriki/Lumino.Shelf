
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


class PageExampleManager
{
public:

	struct LangInfo
	{
		String					extName;
		String					tabName;
		String					targetTabIds;
		RefPtr<StringWriter>	text;
	};

	PageExampleManager()
	{
		m_langInfoList.Resize(5);
		m_langInfoList[0].extName = _T("cpp");
		m_langInfoList[0].tabName = _T("C++");
		m_langInfoList[1].extName = _T("cs");
		m_langInfoList[1].tabName = _T("C#");
		m_langInfoList[2].extName = _T("rb");
		m_langInfoList[2].tabName = _T("Ruby");
		m_langInfoList[3].extName = _T("c");
		m_langInfoList[3].tabName = _T("C");
		m_langInfoList[4].extName = _T("hsp");
		m_langInfoList[4].tabName = _T("HSP");
		Reset();
	}

	void Reset()
	{
		m_current = -1;
		m_tabSectionCount = 0;
		for (auto& info : m_langInfoList)
		{
			info.text = RefPtr<StringWriter>::MakeRef();
		}
	}

	void SetCurrent(const String& extName)
	{
		m_current = m_langInfoList.IndexOf([extName](const LangInfo& info) { return info.extName == extName; });
		m_tabSectionCount++;

		if (m_current >= 0)
		{
			m_langInfoList[m_current].text->WriteLine(_T("```{0}"), extName);
		}
	}

	void WriteLine(const String& line)
	{
		m_langInfoList[m_current].text->WriteLine(line);
	}

	void Export(TextWriter* writer, int paneIndex)
	{
		String cppTabId, cTabId, csTabId, rubyTabId, hspTabId;
		for (int i = 0; i < m_tabSectionCount; ++i)
		{
			if (!cppTabId.IsEmpty()) cppTabId += _T(",");
			cppTabId += String::Format(_T("#cpp{0}"), i + 1);
			if (!cTabId.IsEmpty()) cTabId += _T(",");
			cTabId += String::Format(_T("#c{0}"), i + 1);
			if (!csTabId.IsEmpty()) csTabId += _T(",");
			csTabId += String::Format(_T("#cs{0}"), i + 1);
			if (!rubyTabId.IsEmpty()) rubyTabId += _T(",");
			rubyTabId += String::Format(_T("#rb{0}"), i + 1);
			if (!hspTabId.IsEmpty()) hspTabId += _T(",");
			hspTabId += String::Format(_T("#hsp{0}"), i + 1);
		}

		writer->WriteLine(_T(R"(<ul id="ln_sync_tabs" class="nav nav-tabs">)"));
		writer->WriteLine(_T(R"(<li class="active"><a href="#cpp-tab" data-target="{0}" data-toggle="tab">C++</a></li>)"), cppTabId);
		writer->WriteLine(_T(R"(<li><a href="#c-tab" data-target="{0}" data-toggle="tab">C</a></li>)"), cTabId);
		writer->WriteLine(_T(R"(<li><a href="#cs-tab" data-target="{0}" data-toggle="tab">C#</a></li>)"), csTabId);
		writer->WriteLine(_T(R"(<li><a href="#rb-tab" data-target="{0}" data-toggle="tab">Ruby</a></li>)"), rubyTabId);
		writer->WriteLine(_T(R"(<li><a href="#hsp-tab" data-target="{0}" data-toggle="tab">HSP</a></li>)"), hspTabId);
		writer->WriteLine(_T(R"(</ul>)"));
		writer->WriteLine(_T(R"(<div id="ln_sync_tab_content" class="tab-content">)"));

		for (int i = 0; i < m_langInfoList.GetCount(); ++i)
		{
			auto& info = m_langInfoList[i];
			if (i == 0)
				writer->WriteLine(_T(R"(<div class="tab-pane fade in active" id="{0}{1}">)"), info.extName, paneIndex + 1);
			else
				writer->WriteLine(_T(R"(<div class="tab-pane fade" id="{0}{1}">)"), info.extName, paneIndex + 1);

			writer->WriteLine(info.text->ToString());

			writer->WriteLine(_T(R"(</div>)"));
		}

		writer->WriteLine(_T(R"(</div>)"));
	}

private:
	Array<LangInfo>	m_langInfoList;
	int				m_current;
	int				m_tabSectionCount;
};


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




	int paneCount = -1;
	bool inTabsSection = false;

	StreamReader reader(m_srcFileFullPath);
	StreamWriter writer(file);
	String line;
	PageExampleManager exampleManager;
	while (reader.ReadLine(&line))
	{
		MatchResult m;

		if (Regex::Search(line, _T(R"(\[:include\((.*)\)\])"), &m))
		{
			PathName path(m_srcFileFullPath.GetParent(), m[1]);
			String t = FileSystem::ReadAllText(path);
			if (inTabsSection)
			{
				exampleManager.WriteLine(t);
			}
			else
			{
				writer.WriteLine(t);
			}
			continue;
		}

		if (!inTabsSection)
		{
			if (line.IndexOf(_T("<sync-tabs>")) >= 0)
			{
				inTabsSection = true;
				paneCount++;
				exampleManager.Reset();
			}
			else
			{
				writer.WriteLine(line);
			}
		}
		else
		{
			if (Regex::Search(line, _T("````*([^`]+)"), &m))
			{
				exampleManager.SetCurrent(m[1]);
			}
			else if (line.IndexOf(_T("</sync-tabs>")) >= 0)
			{
				exampleManager.Export(&writer, paneCount);
				exampleManager.Reset();
				paneCount = 0;
				inTabsSection = false;
			}
			else
			{
				exampleManager.WriteLine(line);
			}
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

