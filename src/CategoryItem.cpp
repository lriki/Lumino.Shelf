
#include "Common.h"
#include "Manager.h"
#include "CategoryManager.h"
#include "CategoryItem.h"
#include "Serializer.h"

//==============================================================================
// CategoryItem
//==============================================================================

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
CategoryItem::CategoryItem(CategoryManager* manager)
	: m_manager(manager)
	, m_parent(nullptr)
{
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CategoryItem::PostSerialize()
{
	// src があればページを読み込んでみる
	if (!m_srcPagePath.IsEmpty())
	{
		Serializer serializer;
		serializer.LoadCategoryIndex(this, m_srcPagePath);
	}
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CategoryItem::AddChild(CategoryItem* child)
{
	m_children.Add(child);
	child->m_parent = this;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
String CategoryItem::GetCaption() const
{
	if (!m_caption.IsEmpty())
	{
		return m_caption;
	}
	if (m_homePage != nullptr)
	{
		return m_homePage->GetCaption();
	}
	return String(_T("invalid caption."));
}

#if 0
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
	
	// "" で囲まれていればキャプション文字とする
	MatchResult result;
	if (Regex::Match(source, _T("\"(.*)\""), &result))
	{
		m_itemSource = CategoryItemSource::Caption;
		m_sourceString = result.GetGroup(1);
	}
	// ファイルであればページのソースファイルとする
	else if (FileSystem::ExistsFile(srcFile))
	{
		m_itemSource = CategoryItemSource::File;
		m_indexPage = PagePtr::MakeRef();
		m_indexPage->Initialize(m_manager->GetManager(), this, srcFile, nullptr);
	}
	// フォルダであれば中に目次ファイルがあるはず
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
	// ファイルとか見つからなかったのでとりあえずキャプション扱いにする
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
#endif
