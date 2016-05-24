#include "Common.h"
#include "Manager.h"
#include "Serializer.h"

const char g_usageString[] =
	"lnshelf <directory_path>";

int main(int argc, char **argv)
{
	setlocale(LC_ALL, "");

	try
	{
#if 1
		Serializer serializer;
		ManagerPtr manager = serializer.LoadManager(_T("C:/Proj/Lumino/doc/shelf/shelf.xml"));

		manager->Build();

		//Manager manager;
		//manager.SetSourceDirectory(_T("C:/Proj/Lumino/doc/shelf"));
		//manager.SetTemplateDirectory(_T("C:/Proj/Lumino.Shelf/doc/templates"));
		//manager.SetReleaseDirectory(_T("C:/Proj/Lumino/doc/shelf/release"));
#else
		manager.SetSourceDirectory(_T("C:/Proj/Lumino.Shelf/doc/src"));
		manager.SetTemplateDirectory(_T("C:/Proj/Lumino.Shelf/doc/templates"));
		manager.SetReleaseDirectory(_T("C:/Proj/Lumino.Shelf/doc/release"));
		manager.Build();
#endif

	}
	catch (Exception& e)
	{
		Console::WriteLine(e.GetMessage());
	}
	return 0;
}
