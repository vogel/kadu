/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STATUS_H
#define STATUS_H

#include <qtimer.h>

#define GG_STATUS_INVISIBLE2 0x0009 /* g³upy... */

extern int gg_statuses[];
extern const char *statustext[];

bool ifStatusWithDescription(int status);
int getActualStatus();
int statusGGToStatusNr(int);

class AutoStatusTimer : public QTimer
{
	Q_OBJECT

	private slots:
		void onTimeout();

	public:
		AutoStatusTimer(QObject *parent = 0);
};

class AutoAwayTimer : public QTimer
{
	Q_OBJECT

	private slots:
		void onTimeout();

	public:
		AutoAwayTimer(QObject *parent = 0);

	protected:
		bool eventFilter(QObject *, QEvent *);

	private:
		bool autoawayed;
		int beforeAutoAway;
};

#endif
