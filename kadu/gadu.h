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

struct SearchResult {
	QString uin;
	QString first;
	QString nick;
	QString born;
	QString city;
	int status;

	SearchResult();
	SearchResult(const SearchResult&);
	void setData(const char *uin, const char *first, const char *nick, const char *born, const char *city, const char *status);
};

typedef QValueList<SearchResult> SearchResults;

struct SearchRecord
{
	int seq;
	int fromUin;
	QString uin;
	QString firstName;
	QString lastName;
	QString nickName;
	QString city;
	QString birthYearFrom;
	QString birthYearTo;
	int gender;
	bool active;

	SearchRecord();
	virtual ~SearchRecord();

	void reqUin(const QString& uin);
	void reqFirstName(const QString& firstName);
	void reqLastName(const QString& lastName);
	void reqNickName(const QString& nickName);
	void reqCity(const QString& city);
	void reqBirthYear(const QString& birthYearFrom, const QString& birthYearTo);
	void reqGender(bool female);
	void reqActive();

	void clearData();

};

class GaduProtocol : public QObject
{
	Q_OBJECT

	public:	
		static void initModule();
		GaduProtocol();
		virtual ~GaduProtocol();
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
		bool sendImage(uin_t uin,const QString& file_name,uint32_t size,char* data);

		/**
		  	Szuka ludzi w katalogu publicznym
		 **/
		void searchInPubdir(SearchRecord& searchRecord);
		void searchNextInPubdir(SearchRecord& searchRecord);

	public slots:
		void sendUserList();
		void newResults(gg_pubdir50_t res);

	signals:
		void newSearchResults(SearchResults& searchResults, int seq, int lastUin);
};

extern GaduProtocol* gadu;

#endif

