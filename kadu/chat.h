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
#include <qtoolbutton.h>
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
		void alertNewMessage(void);
      
		KTextBrowser *body;
		CustomInput *edit;
		QHBox *buttontray;
		bool autosend_enabled;
    
	public slots:
		void HistoryBox(void);
		void sendMessage(void);
		void cleanUp(void);
		void writeMyMessage(void);
		void changeTitle(void);

	protected:
		void closeEvent(QCloseEvent *);
		QString convertCharacters(QString);
		virtual void windowActivationChange(bool oldActive);
		void keyPressEvent(QKeyEvent *e);

	private slots:
		void userWhois(void);
		void insertEmoticon(void);
		void regAutosend(void);
		void addMyMessageToHistory(void);

  private:
    QString title_buffer;
    QTimer *title_timer;  
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

class IconSelectorButton : public QToolButton
{
	Q_OBJECT
	
	private:
		QString EmoticonString;
	
	private slots:
		void buttonClicked();
	
	public:
		IconSelectorButton(QWidget* parent,const QString& emoticon_string);	
	
	signals:
		void clicked(const QString& emoticon_string);
};

class IconSelector : public QWidget {
	Q_OBJECT
	public:
		IconSelector(QWidget* parent = 0, const char *name = 0, Chat *caller = 0);

	private:
		Chat *callingwidget;

	private slots:
		void iconClicked(const QString& emoticon_string);
};

#endif
