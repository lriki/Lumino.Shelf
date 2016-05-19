
#include "Common.h"
#include "Serializer.h"

//==============================================================================
// Serializer
//==============================================================================

//------------------------------------------------------------------------------
ManagerPtr Serializer::LoadManager(const PathName& xmlFileFullPath)
{
	XmlFileReader reader(xmlFileFullPath);
	while (reader.Read())
	{
		if (reader.GetNodeType() == XmlNodeType::Element && reader.GetName() == _T("shelf"))
		{
			return Load_shelf(&reader);
		}
	}
	return nullptr;
}

//------------------------------------------------------------------------------
void Serializer::LoadCategoryIndex(CategoryItem* category, const PathName& xmlFileFullPath)
{
	m_manager = category->GetCategoryManager()->GetManager();

	XmlFileReader reader(xmlFileFullPath);
	while (reader.Read())
	{
		if (reader.GetNodeType() == XmlNodeType::Element && reader.GetName() == _T("category_index"))
		{
			Load_category_index(&reader, category);
		}
	}
}

//------------------------------------------------------------------------------
ManagerPtr Serializer::Load_shelf(XmlFileReader* reader)
{
	auto ptr = ManagerPtr::MakeRef();
	m_manager = ptr;
	m_manager->SetSourceDirectory(reader->GetFilePath().GetParent());
	m_manager->SetTemplateDirectory(_T("C:/Proj/Lumino.Shelf/doc/templates"));	// TODO
	m_manager->SetReleaseDirectory(_T("C:/Proj/Lumino/doc/shelf/release"));		// TODO

	// child elements
	if (reader->IsEmptyElement()) return ptr;
	while (reader->Read())
	{
		if (reader->GetNodeType() == XmlNodeType::EndElement) return ptr;

		if (reader->GetNodeType() == XmlNodeType::Element && reader->GetName() == _T("categories"))
		{
			ptr->m_categoryManager = Load_categories(reader);
		}
		if (reader->GetNodeType() == XmlNodeType::Element && reader->GetName() == _T("homepage"))
		{
			ptr->m_homePage = Load_homepage(reader, nullptr);
		}
	}

	return ptr;
}

//------------------------------------------------------------------------------
PagePtr Serializer::Load_homepage(XmlFileReader* reader, CategoryItem* ownerCategory)
{
	// child elements
	if (reader->IsEmptyElement()) return nullptr;
	while (reader->Read())
	{
		if (reader->GetNodeType() == XmlNodeType::EndElement) return nullptr;

		if (reader->GetNodeType() == XmlNodeType::Element && reader->GetName() == _T("page"))
		{
			return Load_page(reader, nullptr, ownerCategory);
		}
	}

	return nullptr;
}

//------------------------------------------------------------------------------
PagePtr Serializer::Load_page(XmlFileReader* reader, Page* parent, CategoryItem* ownerCategory)
{
	auto ptr = PagePtr::MakeRef(m_manager, ownerCategory);

	// attributes
	if (reader->MoveToFirstAttribute())
	{
		do
		{
			if (reader->GetName() == _T("src")) ptr->m_srcFullPath = PathName(reader->GetFilePath().GetParent(), reader->GetValue());
			if (reader->GetName() == _T("template")) ptr->m_templateFullPath = PathName(reader->GetFilePath().GetParent(), reader->GetValue());
			if (reader->GetName() == _T("caption")) ptr->m_caption = reader->GetValue();

		} while (reader->MoveToNextAttribute());

		reader->MoveToElement();
	}
	ptr->PostSerialize();

	// child elements
	if (reader->IsEmptyElement()) return ptr;
	while (reader->Read())
	{
		if (reader->GetNodeType() == XmlNodeType::EndElement) return ptr;

		if (reader->GetNodeType() == XmlNodeType::Element && reader->GetName() == _T("page"))
		{
			ptr->AddChild(Load_page(reader, ptr, ownerCategory));
		}
	}

	return ptr;
}

//------------------------------------------------------------------------------
CategoryManagerPtr Serializer::Load_categories(XmlFileReader* reader)
{
	auto ptr = CategoryManagerPtr::MakeRef(m_manager);

	// child elements
	if (reader->IsEmptyElement()) return ptr;
	while (reader->Read())
	{
		if (reader->GetNodeType() == XmlNodeType::EndElement) return ptr;

		if (reader->GetNodeType() == XmlNodeType::Element &&
			reader->GetName() == _T("category"))
		{
			ptr->m_rootCategoryList.Add(Load_category(reader, ptr, nullptr));
		}
	}

	return ptr;
}

//------------------------------------------------------------------------------
CategoryItemPtr Serializer::Load_category(XmlFileReader* reader, CategoryManager* manager, CategoryItem* parent)
{
	auto ptr = CategoryItemPtr::MakeRef(manager);

	// attributes
	if (reader->MoveToFirstAttribute())
	{
		do
		{
			if (reader->GetName() == _T("src")) ptr->m_srcPagePath = PathName(reader->GetFilePath().GetParent(), reader->GetValue());
			if (reader->GetName() == _T("caption")) ptr->m_caption = reader->GetValue();

		} while (reader->MoveToNextAttribute());

		reader->MoveToElement();
	}
	ptr->PostSerialize();

	// child elements
	if (reader->IsEmptyElement()) return ptr;
	while (reader->Read())
	{
		if (reader->GetNodeType() == XmlNodeType::EndElement) return ptr;

		if (reader->GetNodeType() == XmlNodeType::Element && reader->GetName() == _T("category"))
		{
			ptr->AddChild(Load_category(reader, manager, ptr));
		}
	}

	return ptr;
}

//------------------------------------------------------------------------------
void Serializer::Load_category_index(XmlFileReader* reader, CategoryItem* category)
{
	// child elements
	if (reader->IsEmptyElement()) return;
	while (reader->Read())
	{
		if (reader->GetNodeType() == XmlNodeType::EndElement) return;

		if (reader->GetNodeType() == XmlNodeType::Element && reader->GetName() == _T("homepage"))
		{
			category->m_homePage = Load_homepage(reader, category);
		}
		if (reader->GetNodeType() == XmlNodeType::Element && reader->GetName() == _T("toc"))
		{
			category->m_toc = Load_toc(reader, category);
		}
	}
}

//------------------------------------------------------------------------------
PageTocPtr Serializer::Load_toc(XmlFileReader* reader, CategoryItem* ownerCategory)
{
	auto ptr = PageTocPtr::MakeRef();

	// child elements
	if (reader->IsEmptyElement()) return ptr;
	while (reader->Read())
	{
		if (reader->GetNodeType() == XmlNodeType::EndElement) return ptr;

		if (reader->GetNodeType() == XmlNodeType::Element && reader->GetName() == _T("page"))
		{
			ptr->AddRootPage(Load_page(reader, nullptr, ownerCategory));
		}
	}

	return ptr;
}
