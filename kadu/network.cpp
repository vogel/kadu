/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qpushbutton.h>
#include <qlayout.h>
#include <qtimer.h>

//
#include "kadu.h"
#include "pixmaps.h"
//

extern int last_ping;
extern bool socket_active;
extern Operation *progresswindow;

	/* progress bar, now isn't the name self-explanatory? :> */
Operation::Operation(const QString & labelText, const QString & cancelButtonText, int totalSteps, QWidget *parent )
: QProgressDialog(labelText, cancelButtonText, totalSteps, parent) {
//    setWFlags(Qt::WDestructiveClose);
    steps = 1;
    connect(this, SIGNAL(cancelled()), this, SLOT(cancel()));
    setLabelText(i18n("Connecting"));
    t = new QTimer(this);
    connect(t, SIGNAL(timeout()), this, SLOT(perform()));
    laststate = 0;
    t->start(50, TRUE);
    setAutoClose(true);
    setAutoReset(true);
}
  
void Operation::perform() {
    if (sess->state == laststate) { 
	t->start(50, TRUE);
	return;
	}

    switch (sess->state) {
	case GG_STATE_RESOLVING:
	    laststate = GG_STATE_RESOLVING;
	    setLabelText(i18n("Resolving address"));
	    setProgress(1);
	    t->start(50, TRUE);
	    break;
	case GG_STATE_CONNECTING_HUB:
	    laststate = GG_STATE_CONNECTING_HUB;
	    setLabelText(i18n("Connecting to hub"));
	    setProgress(2);
	    t->start(50, TRUE);
	    break;
	case GG_STATE_READING_DATA:
	    laststate = GG_STATE_READING_DATA;
	    setLabelText(i18n("Fetching data from hub"));
	    setProgress(3);
	    t->start(50, TRUE);
	    break;
	case GG_STATE_CONNECTING_GG:
	    laststate = GG_STATE_CONNECTING_GG;
	    setLabelText(i18n("Connecting to server"));
	    setProgress(4);
	    t->start(50, TRUE);
	    break;
	case GG_STATE_READING_KEY:
	    laststate = GG_STATE_READING_KEY;
	    setLabelText(i18n("Waiting for hash key"));
	    setProgress(5);
	    t->start(50, TRUE);
	    break;
	case GG_STATE_READING_REPLY:
	    laststate = GG_STATE_READING_KEY;
	    setLabelText(i18n("Sending key"));
	    setProgress(6);
	    t->start(50, TRUE);
	    break;
	default:
	    setProgress(7);
	    break;
	}
}

void Operation::cancel() {
    kadu->disconnectNetwork();
    kadu->setCurrentStatus(GG_STATUS_NOT_AVAIL);
    t->stop();
    progresswindow = NULL;
}

