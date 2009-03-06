/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef OPEN_CHAT_WITH_H
#define OPEN_CHAT_WITH_H

#include <QtGui/QWidget>

#include "gui/widgets/contacts-list-widget.h"

#include "open-chat-with-runner.h"

class QLabel;
class QLineEdit;
class QPushButton;
class QVBoxLayout;

/**
	Klasa reprezentuje okno umozliwiajace rozpoczecie rozmowy z dowolnym uzytkownikiem wybranym z listy kontaktów lub podanym przez jego identyfikator.
	\class ContactDataWindow
	\brief Okno dialogowe otwarcia rozmowy z dowolnym kontaktem
**/
class KADUAPI OpenChatWith : public QWidget
{
	Q_OBJECT

	ContactsListWidget *ContactsWidget;
	QLineEdit *ContactID;
	QVBoxLayout *MainLayout;
	OpenChatWithRunner *OpenChatRunner;

private slots:
	void inputAccepted();
	void inputChanged(const QString &text);
	void openChat(Contact contact);

protected:
	virtual void keyPressEvent(QKeyEvent *e);

public:
	OpenChatWith(QWidget *parent = 0);
	virtual ~OpenChatWith();

};

#endif // OPEN_CHAT_WITH_H
