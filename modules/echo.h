#ifndef ECHO_H
#define ECHO_H

#include <qobject.h>
#include "misc.h"

class Echo : public QObject
{
	Q_OBJECT

	public:	
		Echo();
		~Echo();
		
	public slots:
		void chatReceived(UinsList senders,const QString& msg,time_t time);
};

extern Echo* echo;

#endif
