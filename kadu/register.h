/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef REGISTER_H
#define REGISTER_H

#include <qdialog.h>
#include <qtimer.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qsocketnotifier.h>
#include "../libgadu/lib/libgadu.h"

class Register : public QDialog {
	Q_OBJECT

	public:
		Register(QDialog* parent=0, const char *name=0);

	private:
		struct gg_http *h;
		struct gg_pubdir *p;
		QLineEdit *pwd, *pwd2, *mail;
		QLabel *status;
		uin_t uin;
		QCheckBox *updateconfig;
		QSocketNotifier *snr;
		QSocketNotifier *snw;

	  	void ask();	
		void deleteSocketNotifiers();

	private slots:
		void doRegister();
		void socketEvent();
		void dataReceived();
		void dataSent();

	protected:
		void closeEvent(QCloseEvent *e);
};

#endif
