/***************************************************************************
                            sms.h  -  description
                             -------------------
    begin                : Sun Dec 2 2001
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

#ifndef SMS_H
#define SMS_H

#include <qdialog.h>
#include <qmultilineedit.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qprocess.h>
#include "../libgadu/lib/libgadu.h"

class Sms : public QDialog {
	Q_OBJECT
	
	public:
		Sms(unsigned int gsmno = 0, const QString & = "sms", QDialog* parent=0);

	private:
		QMultiLineEdit *body;
		QLineEdit *recipient;
		QLabel *smslen;
		QPushButton *b_send;
		QProcess *smsProcess;

	private slots:
		void updateRecipient(const QString &);
		int sendSms();
		void updateCounter();
		void smsSigHandler();
};

#endif
