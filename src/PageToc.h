#pragma once

class PageToc;
typedef RefPtr<PageToc> PageTocPtr;

class PageToc
	: public Object
{
public:
	void Initialize(const PathName& tocFilePath);

private:
};
