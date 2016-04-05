
#include "Common.h"
#include "Manager.h"
#include "CategoryManager.h"
#include "CategoryItem.h"

//==============================================================================
// CategoryItem
//==============================================================================

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CategoryItem::Initialize(CategoryManager* manager, CategoryItem* parent, const String& source)
{
	m_manager = manager;
	m_parent = parent;
	m_sourceString = source;
	PathName srcDir = m_manager->GetManager()->GetSourceDirectory();
	PathName srcFile(srcDir, source);
	
	// "" �ň͂܂�Ă���΃L���v�V���������Ƃ���
	if (Regex::Match(source, _T("\"(.*)\"")))
	{
		m_itemSource = CategoryItemSource::Caption;
	}
	// �t�@�C���ł���΃y�[�W�̃\�[�X�t�@�C���Ƃ���
	else if (FileSystem::ExistsFile(srcFile))
	{
		m_itemSource = CategoryItemSource::File;
		m_indexPage = PagePtr::MakeRef();
		m_indexPage->Initialize(m_manager->GetManager(), this, srcFile);
	}
	// �t�H���_�ł���Β��ɖڎ��t�@�C��������͂�
	else if (FileSystem::ExistsDirectory(srcFile))
	{
		m_itemSource = CategoryItemSource::Directory;

		// index
		PathName indexPath(srcFile, _T("index.md"));
		m_indexPage = PagePtr::MakeRef();
		m_indexPage->Initialize(m_manager->GetManager(), this, indexPath);

		// toc
		PathName tocPath(srcFile, _T("toc.md"));
		if (tocPath.ExistsFile())
		{
			ReadTocFile(tocPath);
		}
		else
		{
			LN_NOTIMPLEMENTED();
		}
	}
	// �t�@�C���Ƃ�������Ȃ������̂łƂ肠�����L���v�V���������ɂ���
	else
	{
		m_itemSource = CategoryItemSource::Caption;
	}
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
String CategoryItem::GetCaption() const
{
	switch (m_itemSource)
	{
	case CategoryItemSource::Caption:
		return m_sourceString;
	case CategoryItemSource::File:
		return m_indexPage->GetCaption();
	case CategoryItemSource::Directory:
		return m_indexPage->GetCaption();
	default:
		break;
	}
	return String(_T("invalid caption"));
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CategoryItem::ReadTocFile(const PathName& filePath)
{
#if 0
	struct StackItem
	{
		PagePtr item;
		int		level;
	};

	Array<PagePtr>& chilren = m_indexPage->GetChildren();

	Stack<StackItem> itemStack;
	int level = 0;
	StreamReader reader(filePath, Encoding::GetUTF8Encoding());
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
				StackItem si = { chilren.GetLast(), spaceLevel };
				itemStack.Push(si);
			}
			level = spaceLevel;

			// ���݂̐e�́H
			Page* parent = nullptr;
			if (!itemStack.IsEmpty())
			{
				parent = itemStack.GetTop().item;
			}

			// Item �쐬
			auto item = PagePtr::MakeRef();
			item->Initialize(this, parent, result.GetGroup(1));

			// ���[�g�v�f�Ȃ烊�X�g�֊o���Ă���
			if (parent == nullptr)
			{
				chilren.Add(item);
			}
			else
			{
				parent->GetChildren().Add(item);
			}
		}
	}
#endif
}