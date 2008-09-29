#ifndef MACOSX_DOCKING
#define MACOSX_DOCKING

#include <QtCore/QObject>
#include <QtGui/QMenu>

#include "configuration_aware_object.h"
#include "main_configuration_window.h"

/**
 * @defgroup mac_os_docking MacOS docking
 * @{
 */
class MacOSXDocking : public ConfigurationUiHandler
{
	Q_OBJECT
	private slots:
		void blockSettingIcon(bool &);
		void trayPixmapChanged(const QIcon &, const QString &);
		void findTrayPosition(QPoint &);
		void onCreateTabGeneral();
		void toggleKaduVisibility();
	public:
		MacOSXDocking(QObject *parent = 0, const char *name = 0);
		~MacOSXDocking();
		
		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
	protected:
		QMenu *menu;

};

extern MacOSXDocking *mac_docking;

/** @} */

#endif

