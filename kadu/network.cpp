/***************************************************************************
                          network.cpp  -  description
                             -------------------
    begin                : Tue Oct 2 2001
    copyright            : (C) 2001 by tomee
    email                : tomee@cpi.pl
 ***************************************************************************/

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
#include <klocale.h>
#include "../libgadu/lib/libgadu.h"

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
    if (sess.state == laststate) { 
	t->start(50, TRUE);
	return;
	}

    switch (sess.state) {
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

ChooseDescription::ChooseDescription ( int nr, QWidget * parent, const char * name)
: QDialog(parent, name, true) {
    setWFlags(Qt::WDestructiveClose);
    setCaption(i18n("Select description"));
    desc = new QMultiLineEdit(this);

    QPushButton * okidoki;
    okidoki = new QPushButton(this);
    okidoki->setText(i18n("&OK"));
//    okidoki->setAccel(QKeySequence("ALT+O"));
    char **gg_xpm;
    switch (nr) {
	case 1:
	    gg_xpm = gg_actdescr_xpm;
	    break;
	case 3:
	    gg_xpm = gg_busydescr_xpm;
	    break;
	case 5:
	    gg_xpm = gg_invidescr_xpm;
	    break;
	case 7:
	    gg_xpm = gg_inactdescr_xpm;
	    break;
	default:
	    gg_xpm = gg_inactdescr_xpm;
	}
    okidoki->setIconSet(QIconSet( QPixmap((const char**)gg_xpm) ));

    QObject::connect(okidoki, SIGNAL( clicked() ), this, SLOT( okidokiPressed() ));

    QGridLayout * grid = new QGridLayout(this, 2,2);

    grid->addMultiCellWidget(desc,0,0,0,1);
    grid->addWidget(okidoki,1,1,Qt::AlignRight);
    grid->addColSpacing(0,200);

    resize(250,150);
    desc->setText(own_description);
    desc->selectAll();
}

void ChooseDescription::okidokiPressed() {
    own_description = desc->text();
    config.defaultdescription = own_description;
    close();
}
