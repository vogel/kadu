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

/**
	Menad¿er zdarzeñ. Udostêpnia sygna³y reprezentuj±ce zdarzenia
	otrzymywane z serwera GG.
**/
class EventManager : public QObject
{
	Q_OBJECT

	private slots:
		void imageRequestReceivedSlot(UinType sender,uint32_t size,uint32_t crc32);
		void imageReceivedSlot(UinType sender,uint32_t size,uint32_t crc32,const QString& filename,const char* data);
		void imageReceivedAndSavedSlot(UinType sender,uint32_t size,uint32_t crc32,const QString& path);
		void ackReceivedSlot(int seq);
		void dccConnectionReceivedSlot(const UserListElement& sender);

	public:
		EventManager(QObject *parent=NULL, const char *name=NULL);
		void eventHandler(gg_session* sess);

	signals:
		/**
			Nawi±zano po³±czenie z serwerem
		**/
		void connected();
		/**
			B³±d po³±czenia z serwerem
		**/
		void connectionFailed(int);
		/**
			Po³±czenie z serwerem zosta³o przerwane
		**/
		void connectionBroken();
		/**
			Otrzymano polecenie roz³±czenia
		**/
		void disconnected();
		/**
			Który¶ z kontaktów zmieni³ swój status
		**/
		void userStatusChanged(struct gg_event*);
		/**
			Przysz³a z serwera lista kontaktów
		**/
		void userlistReceived(struct gg_event *);
		/**
			Otrzymano wiadomo¶æ systemowa od serwera GG
		**/
		void systemMessageReceived(QString &msg, QDateTime &, int formats_length, void *formats);
		/**
			Otrzymano jak±¶ wiadomo¶æ od serwera GG
		**/
		void messageReceived(int,UinsList,QCString& msg,time_t,
			QByteArray& formats);
		/**
			Otrzymano pro¶bê o wys³anie danych obrazka
		**/
		void imageRequestReceived(UinType sender,uint32_t size,uint32_t crc32);
		/**
			Otrzymano dane obrazka
		**/
		void imageReceived(UinType sender,uint32_t size,uint32_t crc32,const QString& filename,const char* data);
		/**
			Otrzymano dane obrazka i zapisano go do pliku.
		**/
		void imageReceivedAndSaved(UinType sender,uint32_t size,uint32_t crc32,const QString& path);
		/**
			Otrzymano potwierdzenie wiadomo¶ci
		**/
		void ackReceived(int seq);
		/**
			Otrzymano DCC CTCP
		**/
		void dccConnectionReceived(const UserListElement& sender);
		/**
			Otrzymano z serwera rezultat operacji wyszukiwania
			w katalogu publicznym
		**/
		void pubdirReplyReceived(gg_pubdir50_t res);
		/**
			Otrzymano z serwera rezultat operacji na li¶cie
			u¿ytkowników
		**/
		void userlistReplyReceived(char type, char *reply);
};

extern EventManager event_manager;

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
