#include "Common.h"
#include "Manager.h"

int main()
{
	Manager manager;
	manager.SetSourceDirectory(_T("C:/Proj/Lumino.Shelf/doc/src"));
	manager.SetTemplateDirectory(_T("C:/Proj/Lumino.Shelf/doc/templates"));
	manager.SetSourceDirectory(_T("C:/Proj/Lumino.Shelf/doc/release"));
	return 0;
}
