#ifndef MACOSX_DOCKING
#define MACOSX_DOCKING

#include <qobject.h>

class MacOSXDocking : public QObject
{
	Q_OBJECT
	private slots:
		void blockSettingIcon(bool &);
		void trayPixmapChanged(const QPixmap &, const QString &);
		void findTrayPosition(QPoint &);
		void onCreateTabGeneral();
	public:
		MacOSXDocking(QObject *parent = 0, const char *name = 0);
		~MacOSXDocking();
};

extern MacOSXDocking *mac_docking;

#endif

