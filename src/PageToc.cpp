
#include "Common.h"
#include "PageToc.h"

//==============================================================================
// PageToc
//==============================================================================

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void PageToc::Initialize(CategoryItem* owner, const PathName& tocFilePath)
{
	m_ownerCategory = owner;
	m_dirFullPath = tocFilePath.GetParent();

	struct StackItem
	{
		PageTocItemPtr	item;
		int				level;
	};

	Stack<StackItem> itemStack;
	int level = 0;
	StreamReader reader(tocFilePath, Encoding::GetUTF8Encoding());
	String line;
	while (reader.ReadLine(&line))
	{
		MatchResult result;
		if (Regex::Match(line, _T(R"(\s*-\s*(.*))"), &result))
		{
			// 行頭のスペースの数を数える
			int spaceLevel = 0;
			for (int i = 0; i < line.GetLength(); ++i)
			{
				if (isspace(line[i]))
					spaceLevel++;
				else
					break;
			}

			// インデントが変わったのでスタック調整
			if (spaceLevel < level)
			{
				itemStack.Pop();
			}
			else if (spaceLevel > level)
			{
				StackItem si = { m_rootItems.GetLast(), spaceLevel };
				itemStack.Push(si);
			}
			level = spaceLevel;

			// 現在の親は？
			PageTocItem* parent = nullptr;
			if (!itemStack.IsEmpty())
			{
				parent = itemStack.GetTop().item;
			}

			// Item 作成
			auto item = PageTocItemPtr::MakeRef();
			item->Initialize(this, result.GetGroup(1));

			// ルート要素ならリストへ覚えておく
			if (parent == nullptr)
			{
				m_rootItems.Add(item);
			}
			else
			{
				parent->AddChild(item);
			}
		}
	}
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
String PageToc::MakeTocTreeText(Page* currentPage)
{
	StringWriter writer;
	writer.WriteLine(_T("<div class=\"col-md-2\">"));
	writer.WriteLine(_T("<ul class=\"list-group\">"));

	for (PageTocItemPtr item : m_rootItems)
	{
		writer.WriteLine(_T("<li class=\"list-group-item\">"));
		writer.WriteLine(_T("<span class=\"tree-toggler glyphicon glyphicon-triangle-bottom\"></span>"));
		writer.WriteLine(String::Format(_T("<label class=\"nav-header\">{0}</label>"), item->GetCaption()));

		writer.WriteLine(_T("<ul class=\"nav nav-list tree-item\">"));
		for (auto& child : item->GetChildren())
		{
			writer.WriteLine(String::Format(_T("<li><a href=\"{0}\">{1}</a></li>"), currentPage->MakeRelativePath(child->GetPage()), child->GetCaption()));
		}
		writer.WriteLine(_T("</ul>"));

		writer.WriteLine(_T("</li>"));
	}

	writer.WriteLine(_T("</ul>"));
	writer.WriteLine(_T("</div>"));
	return writer.ToString();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void PageToc::MakeTocTreeItemText(StringWriter* writer, PageTocItem* item)
{

}
