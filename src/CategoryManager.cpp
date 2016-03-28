
#include "Common.h"
#include "CategoryManager.h"

//==============================================================================
// CategoryManager
//==============================================================================

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CategoryManager::Initialize(Manager* manager, const PathName& categoriesFile)
{
	m_manager = manager;

	Stack<CategoryItemPtr> itemStack;
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

			// Item �쐬
			auto item = CategoryItemPtr::MakeRef();
			item->Initialize(this, nullptr, result.GetGroup(1));
			m_rootCategoryList.Add(item);
		}
	}
}
