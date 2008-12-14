#ifndef MACOSX_DOCKING
#define MACOSX_DOCKING

#include <QtCore/QObject>
#include <QtGui/QMenu>

#include "configuration_aware_object.h"
#include "main_configuration_window.h"

class UserListElement;
class QPixmap;

/**
 * @defgroup mac_os_docking MacOS docking
 * @{
 */
class MacOSXDocking : public ConfigurationUiHandler
{
	Q_OBJECT
	private:
		bool isBouncing;
		bool overlayed;
		QPixmap pixmap;

		void startBounce();
		void stopBounce();
		void overlay(const QString& text);
	private slots:
		void blockSettingIcon(bool &);
		void trayPixmapChanged(const QIcon &, const QString &);
		void findTrayPosition(QPoint &);
		void onCreateTabGeneral();
		void toggleKaduVisibility();
		void messageListChanged(UserListElement ule);
	public:
		MacOSXDocking(QObject *parent = 0, const char *name = 0);
		~MacOSXDocking();

		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
};

extern MacOSXDocking *mac_docking;

/** @} */

#endif

