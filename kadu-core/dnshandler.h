/*
 * %kadu copyright begin%
 * Copyright 2006, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2008 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KADU_DNS_HANDLER_H
#define KADU_DNS_HANDLER_H

#include <QtNetwork/QHostInfo>

/**
	T�umaczy adres IP na nazw� domeny (DNS). Kiedy proces zostanie zako�czony,
	emituje odpowiedni sygna� zawieraj�cy informacj� o nazwie domeny.
	\class DNSHandler
	\brief Klasa t�umacz�ca adres IP na nazw� domeny.
**/
class DNSHandler : public QObject
{
	Q_OBJECT

	QString marker; /*!< znacznik (np. identyfikator protoko�u) */

	/**
		\fn void resultsReady()
		Funkcja wywo�ywana, gdy proces t�umaczenia zosta� zako�czony.
	**/
	void resultsReady(QHostInfo hostInfo);

public:
	/**
		\fn DNSHandler(const QString &marker, const QHostAddress &addr)
		Konstruktor wywo�uj�cy zapytanie o domen� dla danego adresu IP.
		\param marker znacznik (np. identyfikator protoko�u)
		\param addr adres IP
	**/
	DNSHandler(const QString &marker, const QHostAddress &addr);

	/**
		\fn ~DNSHandler()
		Destruktor klasy
	**/
	~DNSHandler();

	static int counter; /*!< licznik obiekt�w tej klasy */

signals:
	/**
		\fn void result(const QString &marker, const QString &hostname)
		Sygna� emitowany, gdy proces t�umaczenia zosta� zako�czony.
		\param marker znacznik (np. identyfikator protoko�u)
		\param hostname nazwa domeny odpowiadaj�ca adresowi IP o kt�ry pytano
	**/
	void result(const QString &marker, const QString &hostname);
};

#endif // KADU_DNS_HANDLER_H
