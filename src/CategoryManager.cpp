
#include "Common.h"
#include "CategoryManager.h"

//==============================================================================
// CategoryManager
//==============================================================================


CategoryManager::CategoryManager(Manager* manager)
	: m_manager(manager)
{
}

#if 0
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CategoryManager::Initialize(Manager* manager, const PathName& categoriesFile)
{
	struct StackItem
	{
		CategoryItemPtr item;
		int				level;
	};

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
			// �s���̃X�y�[�X�̐��𐔂���
			int spaceLevel = 0;
			for (int i = 0; i < line.GetLength(); ++i)
			{
				if (isspace(line[i]))
					spaceLevel++;
				else
					break;
			}

			// �C���f���g���ς�����̂ŃX�^�b�N����
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

			// ���݂̐e�́H
			CategoryItem* parent = nullptr;
			if (!itemStack.IsEmpty())
			{
				parent = itemStack.GetTop().item;
			}

			// Item �쐬
			auto item = CategoryItemPtr::MakeRef();
			item->Initialize(this, parent, result.GetGroup(1));

			// ���[�g�v�f�Ȃ烊�X�g�֊o���Ă���
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
#endif

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
String CategoryManager::MakeNavBarListText(Page* curPage) const
{
	/*
		������Ȋ����̂����
		<li><a href="./index.html">(�g�b�v�y�[�W)</a></li>
		<li class="active"><a href="./download/index.html">�_�E�����[�h</a></li>
		<li class="dropdown">
		<a href="#" class="dropdown-toggle" data-toggle="dropdown">�T���v��<b class="caret"></b></a>
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
	// <a>�`</a> �����
	String linkSpan;
	if (item->GetHomePage() != nullptr)
	{
		// root �ɖ߂��āA�w��̃y�[�W�Ɉړ�����悤�ȃp�X�ō��
		String relPath = String::Format(_T("{0}/{1}"), curPage->GetRelPathToRoot(), item->GetHomePage()->GetOutputRelPath());
		linkSpan = String::Format(_T("<a href=\"{0}\">{1}</a>"), relPath.Replace(_T("\\"), _T("/")), item->GetHomePage()->GetCaption());
	}
	else
	{
		linkSpan = String::Format(_T("<a>{0}</a>"), item->GetCaption());
	}

	// �q�v�f�̗L���ɂ���� <li> ���H�v����
	if (item->GetChildren().IsEmpty())
	{
		writer->WriteLine(_T("<li>{0}</li>"), linkSpan);
	}
	else
	{
		// �q�v�f�������Ă���̂Ńh���b�v�_�E���ɂ���
		writer->WriteLine(_T("<li class=\"dropdown\">"));
		writer->WriteLine(_T("<a href=\"#\" class=\"dropdown-toggle\" data-toggle=\"dropdown\">{0}<b class=\"caret\"></b></a>"), item->GetCaption());
		writer->WriteLine(_T("<ul class=\"dropdown-menu\">"));

		// �q�v�f���ċN�ŏ���
		for (CategoryItemPtr child : item->GetChildren())
		{
			MakeNavBarItemText(writer, curPage, child, child == curPage->GetOwnerCategory());
		}

		writer->WriteLine(_T("</ul>"));
		writer->WriteLine(_T("</li>"));
	}

}
