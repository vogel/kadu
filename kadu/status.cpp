/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "status.h"
#include "libgadu.h"
#include "gadu.h"
#include <qobject.h>

QString gg_icons[] = {"Online", "OnlineWithDescription", "Busy", "BusyWithDescription", "Invisible", "InvisibleWithDescription",
	"Offline", "OfflineWithDescription", "Blocking"};

int gg_statuses[] = {GG_STATUS_AVAIL, GG_STATUS_AVAIL_DESCR, GG_STATUS_BUSY, GG_STATUS_BUSY_DESCR,
	GG_STATUS_INVISIBLE, GG_STATUS_INVISIBLE_DESCR, GG_STATUS_NOT_AVAIL, GG_STATUS_NOT_AVAIL_DESCR,
	GG_STATUS_BLOCKED};

const char *statustext[] = {
	QT_TR_NOOP("Online"),
	QT_TR_NOOP("Online (d.)"),
	QT_TR_NOOP("Busy"),
	QT_TR_NOOP("Busy (d.)"),
	QT_TR_NOOP("Invisible"),
	QT_TR_NOOP("Invisible (d.)"),
	QT_TR_NOOP("Offline"),
	QT_TR_NOOP("Offline (d.)"),
	QT_TR_NOOP("Blocking")
};

/* our own description container */
QString own_description;
QStringList defaultdescriptions;

/* sprawdza czy nasz status jest opisowy
 odporne na podanie status'u z maska dla przyjaciol */
bool ifStatusWithDescription(int status) {
	status = status & (~GG_STATUS_FRIENDS_MASK);

	return (status == GG_STATUS_AVAIL_DESCR || status == GG_STATUS_NOT_AVAIL_DESCR ||
		status == GG_STATUS_BUSY_DESCR || status == GG_STATUS_INVISIBLE_DESCR);
}

bool isAvailableStatus(unsigned int status)
{
	return (status != GG_STATUS_NOT_AVAIL && status != GG_STATUS_NOT_AVAIL_DESCR);
}

/* zwraca nasz aktualny status 
 jesli stan sesji jest inny niz polaczone to znaczy
 ze jestesmy niedostepni */
int getActualStatus() {
	if (sess && sess->state == GG_STATE_CONNECTED)
		return sess->status;

	return GG_STATUS_NOT_AVAIL;
}

int statusGGToStatusNr(int status) {
	int i = 0;
	if (status == GG_STATUS_INVISIBLE2)
		return 4;
	while (i < 9 && gg_statuses[i] != status)
		i++;
	if (i < 9)
		return i;
	return -1;
}
