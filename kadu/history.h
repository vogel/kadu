/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef HISTORY_H
#define HISTORY_H

#include <qdialog.h>
#include <qmultilineedit.h>
#include "libgadu.h"

class History : public QDialog {
	Q_OBJECT
	public:
		History(UinsList uins);

	protected:
		QMultiLineEdit *body;
};

void appendHistory(UinsList uins,uin_t uin,unsigned char* msg,bool own,time_t=0);
void appendSMSHistory(const QString& mobile,const QString& msg);

#endif
