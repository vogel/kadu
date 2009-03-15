#ifndef GG_AVATARS_H
#define GG_AVATARS_H

#include <QtCore/QObject>
#include <QtCore/QEventLoop>
#include <QtNetwork/QHttp>

#include "configuration_aware_object.h"
#include "main_configuration_window.h"

class QHttp;
class GaduAvatars : public ConfigurationUiHandler, ConfigurationAwareObject
{
    Q_OBJECT
	private:
		QMap<int, QString> avatars;

	public:
		GaduAvatars();
		~GaduAvatars();
		QString getAvatar(const UserListElement &ule);

		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	protected:
		virtual void configurationUpdated();
};

/* 
    Code taken from: 
    http://www.erata.net/qt-boost/synchronous-http-request/
*/
class SyncHTTP: public QHttp
{
	Q_OBJECT
	int requestID;
	bool status;
	QEventLoop loop;
 
public:

	SyncHTTP(QObject * parent = 0) 
		: QHttp(parent), requestID(-1), status(false) {}
	
	SyncHTTP(const QString & hostName, quint16 port = 80, QObject *parent = 0) 
		: QHttp(hostName, port, parent), requestID(-1), status(false) {}
	
	virtual ~SyncHTTP() {}

	bool syncGet(const QString & path, QIODevice *to = 0)
	{
		connect(this, SIGNAL(requestFinished(int,bool)), SLOT(finished(int,bool)));
		requestID = get(path, to);
		loop.exec();
		return status;
	}

protected slots:
	virtual void finished(int idx, bool err)
	{
		if (idx != requestID) return;
		status = !err;
		loop.exit();
	}
};

extern GaduAvatars *gaduAvatars;
#endif
