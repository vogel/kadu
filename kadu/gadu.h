#ifndef GADU_H
#define GADU_H

#include <qstring.h>
#include <qobject.h>
#include "libgadu.h"

#include "misc.h"

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
		int sendMessage(const UinsList& uins,char* msg);
		/**
			Wysyla wiadomosc z formatowaniem tekstu.
			Jesli adresatow jest wiecej niz
			jeden wysylana jest wiadomosc konferencyjna.
			Zwracany jest numer sekwencyjny wiadomosci, jesli
			przypadkiem mysli chcieli sledzic jej potwierdzenie.
		**/
		int sendMessageRichText(const UinsList& uins,char* msg,unsigned char* myLastFormats,int myLastFormatsLength);

	public slots:
		void sendUserList();	
};

extern GaduProtocol* gadu;

#endif
