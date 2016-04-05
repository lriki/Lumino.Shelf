
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
	String stdErr;
	int exitCode = Process::Execute(_T("pandoc"), args, nullptr, &stdErr);	// TODO: �W���o�͂̃G���R�[�f�B���O���w�肵�����E�E�E�B�Ƃ肠�������͈�x�t�@�C���ɗ��Ƃ�
	if (exitCode != 0)
	{
		Console::WriteLineError(stdErr);
		LN_THROW(0, InvalidOperationException);
	}

	// �ꎞ�t�@�C�����J���Ȃ����ĉ�͂���
	StreamReader reader(_T("tmp"), Encoding::GetUTF8Encoding());
	StringWriter writer;
	String line;
	while (reader.ReadLine(&line))
	{
		// <h1> �̓L���v�V����
		MatchResult m1;
		if (Regex::Match(line, _T("<h1 id=\"(.*)\">(.*)</h1>"), &m1))
		{
			m_caption = m1[2];
		}
		// <h2> ���W�߂Ă���
		MatchResult m;
		if (Regex::Match(line, _T("<h2 id=\"(.*)\">(.*)</h2>"), &m))
		{
			m_linkElementList.Add(LinkElement{ m[1], m[2] });
		}

		writer.WriteLine(line);
	}
	m_pageContentsText = writer.ToString();

	// �i�r�Q�[�V����
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
	//if (m_ownerCategory->m_toc != nullptr) {
	//	tocTree = m_ownerCategory->m_toc->MakeTocTree(this);
	//}

	int contentsCols = 8;

	StringWriter contentsText;
	contentsText.WriteLine(_T("<div class=\"col-md-{0}\">"), contentsCols);
	contentsText.WriteLine(_T("<div class=\"lnpc\">"));
	contentsText.WriteLine(m_pageContentsText);
	contentsText.WriteLine(_T("</div>"));
	contentsText.WriteLine(_T("</div>"));
	
	String pageText = m_manager->GetPageTemplateText();
	pageText = pageText.Replace(_T("$(LN_TO_ROOT_PATH)"), m_relpathToRoot);
	pageText = pageText.Replace(_T("$(NAVBAR_ITEMS)"), m_manager->GetCategoryManager()->MakeNavBarListText(this));
	//pageText = pageText.Replace(_T("TOC_TREE"), tocTree);
	pageText = pageText.Replace(_T("$(PAGE_CONTENTS)"), contentsText.ToString());
	pageText = pageText.Replace(_T("$(PAGE_SIDENAV)"), m_pageSideNavText);

	FileSystem::CreateDirectory(m_outputFileFullPath.GetParent());
	FileSystem::WriteAllText(m_outputFileFullPath.c_str(), pageText, Encoding::GetUTF8Encoding());
}
