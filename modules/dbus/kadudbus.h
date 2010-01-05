#ifndef KADU_DBUS_H
#define KADU_DBUS_H

#include <QtCore/QObject>

class KaduDBus : public QObject
{
	Q_OBJECT

	public:
		KaduDBus();
		~KaduDBus();
};

extern KaduDBus* dbus;

#endif
