#ifndef GADU_H
#define GADU_H

#include <qstring.h>
#include <qobject.h>
#include "libgadu.h"

class GaduProtocol : public QObject
{
	Q_OBJECT

	public:	
		static void initModule();
		GaduProtocol();
		
	public slots:
		void sendUserList();	
};

extern GaduProtocol* gadu;

#endif
