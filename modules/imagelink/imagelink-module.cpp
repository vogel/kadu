#include "gui/windows/main-configuration-window.h"
#include "debug.h"
#include "misc/path-conversion.h"
#include "imagelink.h"


extern "C" KADU_EXPORT int imagelink_init()
{
	kdebugf();
	imageLink = new ImageLink();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/imagelink.ui"));
	kdebugf2();
	return 0;
}


extern "C" KADU_EXPORT void imagelink_close()
{
	kdebugf();
	delete imageLink;
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/imagelink.ui"));
	imageLink = NULL;
	kdebugf2();
}
