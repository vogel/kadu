#ifndef AUTORESPONDER_H
#define AUTORESPONDER_H

#include <qobject.h>
#include "misc.h"
#include "config_file.h"

class AutoResponder : public QObject
{
	Q_OBJECT
	
	ConfigFile *config;
	
	public:	
		AutoResponder();
		~AutoResponder();
	public slots:
		void chatReceived(UinsList senders,const QString& msg,time_t time);
		void chatOpened(const UinsList& senders);
	public:
		UinsList UserList;		/* tu beda przechowywane uiny tych ktorzy sie odezwali do nas */
};

extern AutoResponder* autoresponder;

#endif
