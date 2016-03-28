
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
	
	if (Regex::Match(source, _T("\"(.*)\"")))
	{
		m_itemSource = CategoryItemSource::Caption;
	}
	else if (FileSystem::ExistsFile(source))
	{
		m_itemSource = CategoryItemSource::Directory;
		m_indexPage = PagePtr::MakeRef();
		m_indexPage->Initialize(m_manager->GetManager(), this, srcDir);
	}
	else if (FileSystem::ExistsDirectory(source))
	{
		m_itemSource = CategoryItemSource::Directory;
	}
	else
	{
		m_itemSource = CategoryItemSource::Caption;
	}
}

