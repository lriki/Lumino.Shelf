#include "Common.h"
#include "Manager.h"

int main()
{
	setlocale(LC_ALL, "");

	Manager manager;
	manager.SetSourceDirectory(_T("C:/Proj/Lumino.Shelf/doc/src"));
	manager.SetTemplateDirectory(_T("C:/Proj/Lumino.Shelf/doc/templates"));
	manager.SetReleaseDirectory(_T("C:/Proj/Lumino.Shelf/doc/release"));
	manager.Build();
	return 0;
}
