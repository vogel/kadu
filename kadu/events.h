/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef EVENTS_H
#define EVENTS_H

#include <qdialog.h>
#include <qstring.h>
#include <qdatetime.h>

#include "libgadu.h"
#include "misc.h"

class SavePublicKey : public QDialog {
	Q_OBJECT
	public:
		SavePublicKey(uin_t uin, QString keyData, QWidget *parent = 0, const char *name = 0);

	private:
		uin_t uin;
		QString keyData;

	private slots:
		void yesClicked();
};

void eventRecvMsg(int, UinsList, unsigned char *,time_t,int=0,struct gg_msg_format * = NULL);
void eventGotUserlist(struct gg_event *);
void eventStatusChange(struct gg_event *);

extern QTime lastsoundtime;

#endif
