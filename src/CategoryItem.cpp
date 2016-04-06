
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
		m_indexPage->Initialize(m_manager->GetManager(), this, srcFile, nullptr);
	}
	// �t�H���_�ł���Β��ɖڎ��t�@�C��������͂�
	else if (FileSystem::ExistsDirectory(srcFile))
	{
		m_itemSource = CategoryItemSource::Directory;

		// index
		PathName indexPath(srcFile, _T("index.md"));
		m_indexPage = PagePtr::MakeRef();
		m_indexPage->Initialize(m_manager->GetManager(), this, indexPath, nullptr);

		// toc
		PathName tocPath(srcFile, _T("toc.md"));
		if (tocPath.ExistsFile())
		{
			m_toc = PageTocPtr::MakeRef();
			m_toc->Initialize(this, tocPath);
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
