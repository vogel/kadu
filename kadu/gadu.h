#ifndef GADU_H
#define GADU_H

#include <qstring.h>
#include <qobject.h>
#include "libgadu.h"

#include "misc.h"

extern struct gg_session* sess;
extern struct gg_login_params loginparams;

extern QSocketNotifier* kadusnr;
extern QSocketNotifier* kadusnw;

extern bool userlist_sent;
extern bool socket_active;
extern int last_read_event;
extern unsigned int server_nr;
extern bool timeout_connected;
extern QTimer* pingtimer;
extern QValueList<QHostAddress> config_servers;
extern bool i_wanna_be_invisible;

class GaduProtocol : public QObject
{
	Q_OBJECT

	public:	
		static void initModule();
		GaduProtocol();
		/**
			Wysyla wiadomosc. bez formatowania tekstu.
			Jesli adresatow jest wiecej niz
			jeden wysylana jest wiadomosc konferencyjna.
			Zwracany jest numer sekwencyjny wiadomosci, jesli
			przypadkiem mysli chcieli sledzic jej potwierdzenie.
		**/
		int sendMessage(const UinsList& uins,const char* msg);
		/**
			Wysyla wiadomosc z formatowaniem tekstu.
			Jesli adresatow jest wiecej niz
			jeden wysylana jest wiadomosc konferencyjna.
			Zwracany jest numer sekwencyjny wiadomosci, jesli
			przypadkiem mysli chcieli sledzic jej potwierdzenie.
		**/
		int sendMessageRichText(const UinsList& uins,const char* msg,unsigned char* myLastFormats,int myLastFormatsLength);
		/**
			Wysy³a pro¶bê o przys³anie obrazka.
		**/
		bool sendImageRequest(uin_t uin,int size,uint32_t crc32);

	public slots:
		void sendUserList();	
};

extern GaduProtocol* gadu;

#endif
