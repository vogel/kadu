#ifndef KADU_DNS_HANDLER_H
#define KADU_DNS_HANDLER_H

#include <QHostInfo>

/**
	T³umaczy adres IP na nazwê domeny (DNS). Kiedy proces zostanie zakoñczony,
	emituje odpowiedni sygna³ zawieraj±cy informacjê o nazwie domeny.
	\class DNSHandler
	\brief Klasa t³umacz±ca adres IP na nazwê domeny.
**/
class DNSHandler : public QObject
{
	Q_OBJECT

	QString marker; /*!< znacznik (np. identyfikator protoko³u) */

	/**
		\fn void resultsReady()
		Funkcja wywo³ywana, gdy proces t³umaczenia zosta³ zakoñczony.
	**/
	void resultsReady(QHostInfo hostInfo);

public:
	/**
		\fn DNSHandler(const QString &marker, const QHostAddress &addr)
		Konstruktor wywo³uj±cy zapytanie o domenê dla danego adresu IP.
		\param marker znacznik (np. identyfikator protoko³u)
		\param addr adres IP
	**/
	DNSHandler(const QString &marker, const QHostAddress &addr);

	/**
		\fn ~DNSHandler()
		Destruktor klasy
	**/
	~DNSHandler();

	static int counter; /*!< licznik obiektów tej klasy */

signals:
	/**
		\fn void result(const QString &marker, const QString &hostname)
		Sygna³ emitowany, gdy proces t³umaczenia zosta³ zakoñczony.
		\param marker znacznik (np. identyfikator protoko³u)
		\param hostname nazwa domeny odpowiadaj±ca adresowi IP o który pytano
	**/
	void result(const QString &marker, const QString &hostname);
};

#endif
