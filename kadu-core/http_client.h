#ifndef KADU_HTTP_CLIENT_H
#define KADU_HTTP_CLIENT_H

#include <QtCore/QByteArray>
#include <QtNetwork/QTcpSocket>

#include "exports.h"

// TODO: replace with QHttp
class KADUAPI HttpClient : public QObject
{
	Q_OBJECT

	QTcpSocket Socket;
	QString Host;
	QString Agent;
	QString Referer;
	QString Path;
	QByteArray Data;
	QByteArray PostData;

	int StatusCode;
	bool HeaderParsed;
	bool FollowRedirect;

	unsigned int ContentLength;
	bool ContentLengthNotFound;

	QMap<QString, QString> Cookies;

private slots:
	void onConnected();
	void onReadyRead();
	void onConnectionClosed(QAbstractSocket::SocketError errorCode);

public:
	HttpClient();
	int status() const;
	const QByteArray & data() const;

	const QString & cookie(const QString &name) const;
	const QMap<QString, QString> & cookies() const;
	void setCookie(const QString & name, const QString &value);

public slots:
	void setHost(const QString &host);
	void setAgent(const QString &agent);
	void get(const QString &path, bool redirectFollow = true);
	void post(const QString &path, const QByteArray &data, bool redirectFollow = true);
	void post(const QString &path, const QString &data, bool redirectFollow = true);

signals:
	void finished();
	void redirected(QString link);
	void error();

};

#endif
