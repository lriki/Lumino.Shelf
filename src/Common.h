#pragma once
#include <LuminoCore.h>
#include <Lumino/Reflection/ReflectionObject.h>
using namespace ln;

class Manager;
class CategoryManager;
class CategoryItem;
typedef RefPtr<CategoryItem>	CategoryItemPtr;
class Page;
typedef RefPtr<Page>	PagePtr;
class PageToc;


enum class CategoryItemSource
{
	Caption,
	File,
	Directory,
};

