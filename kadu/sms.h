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
#include <qcombobox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qprocess.h>
#include "../libgadu/lib/libgadu.h"

class Sms : public QDialog {
	Q_OBJECT
	
	public:
		Sms(const QString& altnick, QDialog* parent=0);

	private:
		QMultiLineEdit *body;
		QLineEdit *recipient;
		QComboBox* list;
		QLabel *smslen;
		QPushButton *b_send;
		QProcess *smsProcess;

	private slots:
		void updateRecipient(const QString &);
		void updateList(const QString& newnumber);
		int sendSms();
		void updateCounter();
		void smsSigHandler();
};

#endif
