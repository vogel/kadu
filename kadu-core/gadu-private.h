#ifndef GADU_PRIVATE_H
#define GADU_PRIVATE_H

#include <qcstring.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qstring.h>

#include "gadu.h"
#include "protocols_manager.h"
#include "userlistelement.h"

class QSocketNotifier;
struct gg_http;
struct gg_session;

// ------------------------------------
//             Notifiers
// ------------------------------------

#pragma GCC visibility push(hidden)

class SocketNotifiers : public QObject
{
	Q_OBJECT

	protected:
		int Fd;
		QSocketNotifier *Snr;
		QSocketNotifier *Snw;

		void createSocketNotifiers();
		void deleteSocketNotifiers();
		void recreateSocketNotifiers();
		virtual void socketEvent() = 0;

	protected slots:
		virtual void dataReceived() = 0;
		virtual void dataSent() = 0;

	public:
		SocketNotifiers(int fd, QObject *parent=0, const char *name=0);
		virtual ~SocketNotifiers();
		virtual void start();
		virtual void stop();
};

class PubdirSocketNotifiers : public SocketNotifiers
{
	Q_OBJECT

	private:
		struct gg_http *H;

	protected:
		virtual void socketEvent();

	protected slots:
		virtual void dataReceived();
		virtual void dataSent();

	public:
		PubdirSocketNotifiers(struct gg_http *, QObject *parent=0, const char *name=0);
		virtual ~PubdirSocketNotifiers();

	signals:
		void done(bool ok, struct gg_http *);
};

class TokenSocketNotifiers : public SocketNotifiers
{
	Q_OBJECT

	private:
		struct gg_http *H;

	protected:
		virtual void socketEvent();

	protected slots:
		virtual void dataReceived();
		virtual void dataSent();

	public:
		TokenSocketNotifiers(QObject *parent=0, const char *name=0);
		virtual ~TokenSocketNotifiers();

		virtual void start();

	signals:
		void gotToken(QString, QPixmap);
		void tokenError();
};

class GaduSocketNotifiers : public SocketNotifiers
{
	Q_OBJECT

	private:
		gg_session *Sess;
		int socketEventCalls;

		void connectionFailed(int);

	protected:
		virtual void socketEvent();

	public slots:
		virtual void dataReceived();
		virtual void dataSent();

	public:
		GaduSocketNotifiers(QObject *parent=0, const char *name=0);
		virtual ~GaduSocketNotifiers();
		void setSession(gg_session *sess);
		void checkWrite();

	signals:
		void ackReceived(int seq, uin_t uin, int status);
		void connected();
		/**
			Otrzymano wiadomo¶æ CTCP.
			Kto¶ nas prosi o po³±czenie dcc, poniewa¿
			jeste¶my za NAT-em.
			TODO: zmieniæ nazwê.
		**/
		void dccConnectionReceived(const UserListElement &);
		void disconnected();
		void error(GaduError);
		void imageReceived(UinType, uint32_t, uint32_t, const QString &filename, const char *data);
		void imageRequestReceived(UinType, uint32_t, uint32_t);
		void messageReceived(int, UserListElements, QCString &, time_t, QByteArray &);
		void pubdirReplyReceived(gg_pubdir50_t);
		void systemMessageReceived(QString &, QDateTime &, int, void *);
		void userlistReceived(const struct gg_event *);
		void userlistReplyReceived(char, char *);
		void userStatusChanged(const struct gg_event *);
};

class GaduProtocolManager : public ProtocolManager
{
	public:
		virtual Protocol *newInstance(const QString &id);
};

#pragma GCC visibility pop

#endif
