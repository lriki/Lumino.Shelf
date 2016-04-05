
#include "Common.h"
#include "CategoryManager.h"

//==============================================================================
// CategoryManager
//==============================================================================

struct StackItem
{
	CategoryItemPtr item;
	int				level;
};

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CategoryManager::Initialize(Manager* manager, const PathName& categoriesFile)
{
	m_manager = manager;

	Stack<StackItem> itemStack;
	int level = 0;
	StreamReader reader(categoriesFile, Encoding::GetUTF8Encoding());
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
				StackItem si = { m_rootCategoryList.GetLast(), spaceLevel };
				itemStack.Push(si);
			}
			level = spaceLevel;

			// 現在の親は？
			CategoryItem* parent = nullptr;
			if (!itemStack.IsEmpty())
			{
				parent = itemStack.GetTop().item;
			}

			// Item 作成
			auto item = CategoryItemPtr::MakeRef();
			item->Initialize(this, parent, result.GetGroup(1));

			// ルート要素ならリストへ覚えておく
			if (parent == nullptr)
			{
				m_rootCategoryList.Add(item);
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
String CategoryManager::MakeNavBarListText(Page* curPage) const
{
	/*
		↓こんな感じのを作る
		<li><a href="./index.html">(トップページ)</a></li>
		<li class="active"><a href="./download/index.html">ダウンロード</a></li>
		<li class="dropdown">
		<a href="#" class="dropdown-toggle" data-toggle="dropdown">サンプル<b class="caret"></b></a>
		<ul class="dropdown-menu">
		<li><a href="./samples_Lumino/index.html">Lumino</a></li>
		<li><a href="./samples_Lumino.Core/index.html">Lumino.Core</a></li>
		</ul>
		</li>
	*/
	StringWriter writer;
	for (CategoryItemPtr item : m_rootCategoryList)
	{
		MakeNavBarItemText(&writer, curPage, item, item == curPage->GetOwnerCategory());
	}
	return writer.ToString();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CategoryManager::MakeNavBarItemText(StringWriter* writer, Page* curPage, CategoryItem* item, bool active) const
{
	// <a>～</a> を作る
	String linkSpan;
	if (item->GetIndexPage() != nullptr)
	{
		// root に戻って、指定のページに移動するようなパスで作る
		String relPath = String::Format(_T("{0}/{1}"), curPage->GetRelPathToRoot(), item->GetIndexPage()->GetOutputRelPath());
		linkSpan = String::Format(_T("<a href=\"{0}\">{1}</a>"), relPath.Replace(_T("\\"), _T("/")), item->GetIndexPage()->GetCaption());
	}
	else
	{
		linkSpan = String::Format(_T("<a>{0}</a>"), item->GetCaption());
	}

	// 子要素の有無によって <li> を工夫する
	if (item->GetChildren().IsEmpty())
	{
		writer->WriteLine(_T("<li>{0}</li>"), linkSpan);
	}
	else
	{
		// 子要素を持っているのでドロップダウンにする
		writer->WriteLine(_T("<li class=\"dropdown\">"));
		writer->WriteLine(_T("<a href=\"#\" class=\"dropdown-toggle\" data-toggle=\"dropdown\">{0}<b class=\"caret\"></b></a>"), item->GetCaption());
		writer->WriteLine(_T("<ul class=\"dropdown-menu\">"));

		// 子要素を再起で処理
		for (CategoryItemPtr child : item->GetChildren())
		{
			MakeNavBarItemText(writer, curPage, child, child == curPage->GetOwnerCategory());
		}

		writer->WriteLine(_T("</ul>"));
		writer->WriteLine(_T("</li>"));
	}

}
