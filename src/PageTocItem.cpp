
#include "Common.h"
#include "CategoryManager.h"
#include "CategoryItem.h"
#include "PageToc.h"
#include "PageTocItem.h"

//==============================================================================
// PageTocItem
//==============================================================================

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void PageTocItem::Initialize(PageToc* toc, const String& src)
{
	m_ownerToc = toc;

	PathName filePath(m_ownerToc->GetDirectoryFullPath(), src);

	// "" �ň͂܂�Ă���΃L���v�V���������Ƃ���
	MatchResult result;
	if (Regex::Match(src, _T("\"(.*)\""), &result))
	{
		m_caption = result[1];
	}
	// �t�@�C���ł���΃y�[�W�̃\�[�X�t�@�C���Ƃ���
	else if (FileSystem::ExistsFile(filePath))
	{
		m_page = PagePtr::MakeRef();
		m_page->Initialize(m_ownerToc->GetOwnerCategory()->GetCategoryManager()->GetManager(), m_ownerToc->GetOwnerCategory(), filePath, nullptr);
	}
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
String PageTocItem::GetCaption() const
{
	if (m_page != nullptr)
	{
		return m_page->GetCaption();
	}
	return m_caption;
}
