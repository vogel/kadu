/***************************************************************************
                          chat.h  -  description
                             -------------------
    begin                : czw cze 27 17:58:52 CEST 2002
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

#ifndef CHAT_H
#define CHAT_H

#include <qdialog.h>
#include <ktextbrowser.h>
#include <qhbox.h>
#include <qmultilineedit.h>
#include <qarray.h>
#include "../libgadu/lib/libgadu.h"
#include "misc.h"
#include "userbox.h"

class IconSelector;

class CustomInput;

class Chat : public QDialog {
	Q_OBJECT
		int index;
		int totaloccurences;
		QString nick;
		QDialog *chatdlg;
		UinsList uins;
		IconSelector *iconsel_ptr;
		QPushButton *iconsel;
		QPushButton *autosend;
		QAccel *acc;
		QPushButton *sendbtn;
		UserBox *userbox;
		QString myLastMessage;

		void pruneWindow(void);

	public:
		Chat(UinsList uins, QWidget *parent = 0);
		~Chat();
		void setTitle(void);
		void formatMessage(bool, QString &, QString &, const char *, QString &);
		void checkPresence(UinsList, QString &, time_t, QString &);
		void addEmoticon(QString);
		void scrollMessages(QString &);

		KTextBrowser *body;
		CustomInput *edit;
		QHBox *buttontray;
		bool autosend_enabled;

	public slots:
		void HistoryBox(void);
		void sendMessage(void);
		void cleanUp(void);
		void playChatSound(void);
		void writeMyMessage(void);

	protected:
		void closeEvent(QCloseEvent *);
		void alertNewMessage(void);
		QString convertCharacters(QString);

	private slots:
		void resetAutoAway(void);
		void userWhois(void);
		void insertEmoticon(void);
		void regAutosend(void);
		void addMyMessageToHistory(void);
};

class CustomInput : public QMultiLineEdit {
	Q_OBJECT
	public:
		CustomInput(Chat *parent = 0, const char *name = 0);
		friend class Chat;

	private:
		Chat *tata;

	protected:
		void keyPressEvent(QKeyEvent *e);
};

class IconSelector : public QWidget {
	Q_OBJECT
	public:
		IconSelector(QWidget* parent = 0, const char *name = 0, Chat *caller = 0);

	private:
		Chat *callingwidget;

	private slots:
		void slot_1_1();
		void slot_1_2();
		void slot_1_3();
		void slot_1_4();

		void slot_2_1();
		void slot_2_2();
		void slot_2_3();
		void slot_2_4();

		void slot_3_1();
		void slot_3_2();
		void slot_3_3();
		void slot_3_4();

		void slot_4_1();
		void slot_4_2();
		void slot_4_3();
		void slot_4_4();

		void slot_5_1();
		void slot_5_2();
		void slot_5_3();
		void slot_5_4();
};

#endif
