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
		AutoConnectionTimer(QObject *parent = 0);

		static AutoConnectionTimer *autoconnection_object;
};

class ConnectionTimeoutTimer : public QTimer {
	Q_OBJECT

	public:
		static void on();
		static void off();
		static bool connectTimeoutRoutine(const QObject *receiver, const char *member);

	private:
		ConnectionTimeoutTimer(QObject *parent = 0);

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
		void connectionTimeoutTimerSlot();
		void userStatusChangedSlot(struct gg_event*);
		void userlistReceivedSlot(struct gg_event *);
		void messageReceivedSlot(int, UinsList,QCString& msg,time_t,
			QByteArray& formats);
		void systemMessageReceivedSlot(QString &msg, QDateTime &time,
			int formats_length, void *formats);
		void chatMsgReceived2Slot(UinsList senders,const QString& msg,time_t time);
		void imageRequestReceivedSlot(UinType sender,uint32_t size,uint32_t crc32);
		void imageReceivedSlot(UinType sender,uint32_t size,uint32_t crc32,const QString& filename,const char* data);
		void imageReceivedAndSavedSlot(UinType sender,uint32_t size,uint32_t crc32,const QString& path);
		void ackReceivedSlot(int seq);
		void dccConnectionReceivedSlot(const UserListElement& sender);
		void pubdirReplyReceivedSlot(gg_pubdir50_t res);
		void userlistReplyReceivedSlot(char type, char *reply);

	public:
		EventManager();
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
			Po³±czenie z serwerem zosta³o przerwane z powodu zbyt d³ugiego
			czasu nieaktywno¶ci w trakcie ³±czenia
		**/
		void connectionTimeout();
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
			Sygnal daje mozliwosc operowania na wiadomoci
			ktora przyszla z serwera jeszcze w jej oryginalnej
			formie przed konwersja na unicode i innymi zabiegami.
			Tresc wiadomosci mozna zmienic grzebiac w buforze msg,
			ale uwaga: mo¿na zepsuæ formatowanie tekstu zapisane
			w formats. Oczywi¶cie je równie¿ mo¿na zmieniaæ, wed³ug
			opisu protoko³u GG ;)
			Mozna tez przerwac dalsza obrobke wiadomo¶ci ustawiajac
			stop na true.
		**/
		void messageFiltering(const UinsList& senders,QCString& msg,
			QByteArray& formats,bool& stop);
		/**
			Otrzymano wiadomo¶æ, któr± trzeba pokazaæ (klasa chat lub msg,
			nadawca nie jest ignorowany, itp)
			Tre¶æ zdeszyfrowana i zdekodowana do unicode.
			Jesli ktorys ze slotow sygna³u chatMsgReceived1 ustawi zmienna
			grab na true to sygnal chatReceived2 nie zostanie wygenerowany.
			Je¶li natomiast zmienna grab zostanie ustawiona przez slot
			chatMsgReceived0, to ¿adna czynno¶æ zwi±zana z obs³ug± tego
			zdarzenia nie zostanie podjêta (tj. wy¶wietlanie wiadomo¶ci
			w oknie, dodanie jej do historii, etc.), poza przekonwertowaniem
			kodowania wiadomo¶ci z CP1250 na Unicode.
		**/
		void chatMsgReceived0(UinsList senders,const QString& msg,time_t time,bool& grab);
		void chatMsgReceived1(UinsList senders,const QString& msg,time_t time,bool& grab);
		void chatMsgReceived2(UinsList senders,const QString& msg,time_t time);
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

extern QTime lastsoundtime;

class EventConfigSlots : public QObject
{
	Q_OBJECT
	public:
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
