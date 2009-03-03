/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QTime>
#include <QtCore/QUrl>

#include "config_file.h"
#include "debug.h"
#include "misc.h"

#include "http_client.h"

HttpClient::HttpClient() :
		Socket(), Host(), Agent(), Referer(), Path(), Data(), PostData(),
		StatusCode(0), HeaderParsed(false), FollowRedirect(true),
		ContentLength(0), ContentLengthNotFound(false), Cookies()
{
	kdebugf();
	connect(&Socket, SIGNAL(connected()), this, SLOT(onConnected()));
	connect(&Socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
	connect(&Socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onConnectionClosed(QAbstractSocket::SocketError)));
	kdebugf2();
}

void HttpClient::onConnected()
{
	kdebugf();
	QString query = PostData.isEmpty() ? "GET": "POST";
	query += ' ';

	if (Path.left(7) != "http://" && config_file.readBoolEntry("Network", "UseProxy"))
		query += "http://" + Host;

	if ((Path.isEmpty() || Path[0] != '/') && Path.left(7) != "http://")
		query += '/';


	query += Path;
	query += " HTTP/1.1\r\n";
	query += "Host: " + Host + "\r\n";

	// use custom agent if defined
	if (!Agent.isEmpty())
		query += "User-Agent: " + Agent + "\r\n";
	else
		query += "User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.4) Gecko/20030617\r\n";

//	query += "Accept: text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8\r\n";
//	query += "Connection: keep-alive\r\n";
	if (!Referer.isEmpty())
		query += "Referer: " + Referer + "\r\n";

	if (!Cookies.isEmpty())
	{
		query += "Cookie: ";
		foreach(const QString &key, Cookies.keys())
		{
			query += key + '=' + Cookies[key];
// 			++it;
// 			if (it != Cookies.end())
				query += "; ";
// 			--it;
		}
		query += "\r\n";
	}

	if (!PostData.isEmpty())
	{
		query += "Content-Type: application/x-www-form-urlencoded\r\n";
		query += "Content-Length: " + QString::number(PostData.size()) + "\r\n";
	}
	query += "\r\n";
	if (!PostData.isEmpty())
		query += QString(PostData);
	kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: Sending query:\n%s\n", qPrintable(query));
	Socket.write(qPrintable(query), query.length());
	kdebugf2();
}

void HttpClient::onReadyRead()
{
	kdebugf();
	int size = Socket.bytesAvailable();
	kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: Data Block Retreived: %i bytes\n", size);
	// Dodaj nowe dane do starych
	char *buf=new char[size];
	Socket.read(buf, size);
	//
//	kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "%s\n",buf);
	//
	int old_size = Data.size();
	Data.resize(old_size + size);
	for(int i = 0; i < size; ++i)
		Data[old_size + i] = buf[i];
	delete buf;
	// Jesli nie mamy jeszcze naglowka
	if (!HeaderParsed)
	{
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: Trying to parse header\n");
		// Kontynuuj odczyt jesli naglowek niekompletny
		QString s = QString(Data);
		int p = s.indexOf("\r\n\r\n");
		if (p < 0)
			return;
		// Dostalismy naglowek,
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: Http header found:\n%s\n", qPrintable(s));
		HeaderParsed = true;
		// Wyci�gamy status
		QRegExp status_regexp("HTTP/1\\.[01] (\\d+)");
		if (status_regexp.indexIn(s) < 0)
		{
			Socket.close();
			emit error();
			return;
		}
		StatusCode = status_regexp.cap(1).toInt();
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: StatusCode: %i\n", StatusCode);
		// StatusCode 302 oznacza przekierowanie.
		if (StatusCode == 302)
		{
			QRegExp location_regexp("Location: ([^\\r\\n]+)");
			if (location_regexp.indexIn(s) < 0)
			{
				Socket.close();
				emit error();
				return;
			}
			QString location = location_regexp.cap(1);
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "Jumping to %s\n", qPrintable(location));
			// czekamy. zbyt szybkie przekierowanie konczy sie
			// czasem petla. nie wiem dlaczego.
			QTime* t = new QTime();
			t->start();
			while (t->elapsed() < 500) {};
			delete t;
			//
			emit redirected(location);

			// follow only if desired
			if (FollowRedirect)
			{
				Socket.close();
				get(location);
				return;
			}
		}
		// Wyci�gamy Content-Length
		QRegExp cl_regexp("Content-Length: (\\d+)");
		ContentLengthNotFound = cl_regexp.indexIn(s) < 0;
		if (ContentLengthNotFound)
		{
			ContentLength = 0;
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: Content-Length not found. We will wait for connection to close.");
		}
		else
		{
			ContentLength = cl_regexp.cap(1).toUInt();
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: Content-Length: %i bytes\n",ContentLength);
		}

		// Wyciagamy ewentualne cookie (dla uproszczenia tylko jedno)
		QRegExp cookie_regexp("Set-Cookie: ([^=]+)=([^;]+);");
		if (cookie_regexp.indexIn(s) >= 0)
		{
			QString cookie_name = cookie_regexp.cap(1);
			QString cookie_val = cookie_regexp.cap(2);
			Cookies.insert(cookie_name, cookie_val);
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: Cookie retreived: %s=%s\n", qPrintable(cookie_name), qPrintable(cookie_val));
		}
		// Wytnij naglowek z Data
		int header_size = p + 4;
		int new_data_size = Data.size() - header_size;
		for(int i = 0; i < new_data_size; ++i)
			Data[i] = Data[header_size+i];
		Data.resize(new_data_size);
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: Header parsed and cutted off from data\n");
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: Header size: %i bytes\n", header_size);
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: New data block size: %i bytes\n", new_data_size);
		// Je�li status jest 100 - Continue to czekamy na dalsze dane
		// (uniewa�niamy ten nag�owek i czekamy na nast�pny)
		if (StatusCode == 100)
		{
			HeaderParsed = false;
			return;
		}
	}
	// Kontynuuj odczyt jesli dane niekompletne
	// lub je�li mamy czeka� na connection close
	if (ContentLength > Data.size() || ContentLengthNotFound)
		return;
	// Mamy cale dane
	kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: All Data Retreived: %i bytes\n", Data.size());
	Socket.close();
	emit finished();
	kdebugf2();
}

void HttpClient::onConnectionClosed(QAbstractSocket::SocketError errorCode)
{
	kdebugf();
	//jesli powodem bylo przekierowanie, nie emitujemy sygnalow
	if (StatusCode == 302)
		return;
	//czy polaczenie zostalo przerwane przez druga strone?
	if (errorCode == QAbstractSocket::RemoteHostClosedError && Socket.state() == QAbstractSocket::ConnectedState)
	{
		if (HeaderParsed && ContentLengthNotFound)
			emit finished();
		else
			emit error();
	}
	kdebugf2();
}

void HttpClient::setHost(const QString &host)
{
	Host = host;
	Cookies.clear();
}

void HttpClient::setAgent(const QString &agent)
{
	Agent = agent;
}

void HttpClient::get(const QString &path, bool redirectFollow)
{
	Referer = Path;
	Path = path;
	Data.resize(0);
	PostData.resize(0);
	HeaderParsed = false;
	FollowRedirect = redirectFollow;

	if(config_file.readBoolEntry("Network", "UseProxy", false))
		Socket.connectToHost(
			config_file.readEntry("Network", "ProxyHost"),
			config_file.readNumEntry("Network", "ProxyPort"));
	else
		Socket.connectToHost(Host, 80);
}

void HttpClient::post(const QString &path, const QByteArray& data, bool redirectFollow)
{
	Referer = Path;
	Path = path;
	Data.resize(0);
	PostData = data;
	HeaderParsed = false;
	FollowRedirect = redirectFollow;

	if(config_file.readBoolEntry("Network", "UseProxy", false))
		Socket.connectToHost(
			config_file.readEntry("Network", "ProxyHost"),
			config_file.readNumEntry("Network", "ProxyPort"));
	else
		Socket.connectToHost(Host, 80);
}

void HttpClient::post(const QString &path, const QString& data, bool redirectFollow)
{
	QByteArray PostData;
	PostData = qPrintable(data);
	post(path, PostData, redirectFollow);
}

int HttpClient::status() const
{
	return StatusCode;
}

const QByteArray & HttpClient::data() const
{
	return Data;
}

const QString & HttpClient::cookie(const QString& name) const
{
	return Cookies[name];
}

const QMap<QString,QString> & HttpClient::cookies() const
{
	return Cookies;
}

void HttpClient::setCookie(const QString &name, const QString &value)
{
	Cookies[name] = value;
}
