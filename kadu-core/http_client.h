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
	QString Referer;
	QString Path;
	QByteArray Data;
	QByteArray PostData;

	int StatusCode;
	bool HeaderParsed;
	bool Unicode;

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
	static QString encode(const QString &text);

	const QString & cookie(const QString &name) const;
	const QMap<QString, QString> & cookies() const;
	void setCookie(const QString & name, const QString &value);

public slots:
	void setHost(const QString &host);
	void setReferer(const QString &referer);
	void get(const QString &path);
	void post(const QString &path, const QByteArray &data);
	void post(const QString &path, const QString &data);
	void useUnicode(bool on);

signals:
	void finished();
	void redirected(QString link);
	void error();

};

#endif
