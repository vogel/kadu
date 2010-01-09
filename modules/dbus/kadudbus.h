#ifndef KADU_DBUS_H
#define KADU_DBUS_H

#include <QtCore/QObject>
#include <QtDBus/QDBusConnection>

class KaduDBus : public QObject
{
	Q_OBJECT

	QDBusConnection connection;

	public:
		KaduDBus();
		~KaduDBus();
};

extern KaduDBus* dbus;

#endif
