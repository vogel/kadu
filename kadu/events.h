#ifndef EVENTS_H
#define EVENTS_H

#include <qdialog.h>
#include <qstring.h>
#include <qtimer.h>
#include <qlistbox.h>
#include <qdatetime.h>

#include "misc.h"

class AutoConnectionTimer : private QTimer {
	Q_OBJECT

	public:
		static void on();
		static void off();

	public slots:
		void doConnect();

	private:
		AutoConnectionTimer(QObject *parent = 0, const char *name=0);

		static AutoConnectionTimer *autoconnection_object;
};

class ConnectionTimeoutTimer : public QTimer {
	Q_OBJECT

	public:
		static void on();
		static void off();
		static bool connectTimeoutRoutine(const QObject *receiver, const char *member);

	private:
		ConnectionTimeoutTimer(QObject *parent = 0, const char *name=0);

		static ConnectionTimeoutTimer *connectiontimeout_object;
};

class EventConfigSlots : public QObject
{
	Q_OBJECT
	public:
		EventConfigSlots(QObject *parent=NULL, const char *name=NULL);
	    static void initModule();

	public slots:
	    void onCreateConfigDialog();
	    void onDestroyConfigDialog();
	    void ifDccEnabled(bool value);
	    void ifDccIpEnabled(bool value);
	    void ifDefServerEnabled(bool value);
	    void useTlsEnabled(bool value);
	    void _Left();
	    void _Right();
	    void _Left2(QListBoxItem *item);
	    void _Right2(QListBoxItem *item);

	    void ifNotifyGlobal(bool toggled);
	    void ifNotifyAll(bool toggled);
};

#endif
