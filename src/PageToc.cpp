
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

