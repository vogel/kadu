#ifndef AUTORESPONDER_H
#define AUTORESPONDER_H

#include <qobject.h>
#include "misc.h"

class AutoResponder : public QObject
{
	Q_OBJECT

	public:	
		AutoResponder();
		
	public slots:
		void chatReceived(UinsList senders,const QString& msg,time_t time);
};

extern AutoResponder* autoresponder;

#endif
