#include "Common.h"
#include "Manager.h"

const char g_usageString[] =
	"lnshelf <directory_path>";

int main(int argc, char **argv)
{
	setlocale(LC_ALL, "");

	Manager manager;
#if 1
	
	manager.SetSourceDirectory(_T("C:/Proj/Lumino/doc/shelf"));
	manager.SetTemplateDirectory(_T("C:/Proj/Lumino.Shelf/doc/templates"));
	manager.SetReleaseDirectory(_T("C:/Proj/Lumino/doc/shelf/release"));
#else
	manager.SetSourceDirectory(_T("C:/Proj/Lumino.Shelf/doc/src"));
	manager.SetTemplateDirectory(_T("C:/Proj/Lumino.Shelf/doc/templates"));
	manager.SetReleaseDirectory(_T("C:/Proj/Lumino.Shelf/doc/release"));
#endif
	manager.Build();
	return 0;
}
