/***************************************************************************
                          message.h  -  description
                             -------------------
    begin                : sro lip 03 09:06:00 CEST 2002
    copyright            : (C) 2002 by chilek
    email                : chilek@chilan.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MESSAGE_H
#define MESSAGE_H

#include <qdialog.h>
#include <qmultilineedit.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include "../libgadu/lib/libgadu.h"
#include "misc.h"

class Message : public QDialog {
	Q_OBJECT
		QPushButton *sendbtn;
		int index;
		QString nicksnd;
		QMultiLineEdit *body;	
		QLabel *nicknamelab;
		QDialog *sendmessage;
		QRadioButton *b_chat;

	public:
    		Message(const QString &, bool tchat=false, QWidget *parent=0, const char *name=0);
		~Message();
		void init(void);

	public slots:
		void gotAck(void);
		void commitSend(void);
		void HistoryBox(void);		

	protected slots:
		void accept();
		void reject();
};

class rMessage : public QDialog {
	Q_OBJECT
	private:
		QString sender;
		QMultiLineEdit *body;	
		QLabel *lejbel;
		QDialog *recvmsg;
		bool tchat;
	
	public:
		rMessage(const QString &, int msgclass, UinsList uins, QString &msg,
			QWidget *parent=0, const char *name=0);
		void init(void);

	public slots:
		void replyMessage(void);
		void openChat(void);
		void cleanUp(void);
};

#endif
