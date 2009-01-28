/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACTS_LIST_WIDGET_H
#define CONTACTS_LIST_WIDGET_H

#include <QtGui/QListView>
#include <QtCore/QDateTime>
#include <QtCore/QTimer>

#include "contacts/contact.h"

class QContextMenuEvent;
class QModelIndex;

class ContactList;
class ContactsListWidgetDelegate;
class KaduMainWindow;

class ContactsListWidget : public QListView
{
	Q_OBJECT

	KaduMainWindow *MainWindow;
	ContactsListWidgetDelegate *Delegate;

	Contact contact(const QModelIndex &index) const;
	void triggerActivate(const QModelIndex &index);

	// Tool tips
	Contact ToolTipContact;
	QTimer ToolTipTimeoutTimer;

	// D&D
	QPoint DragStartPosition;
	QDateTime DragStartTime;

private slots:
	void doubleClickedSlot(const QModelIndex &index);

	// Tool tips
	void toolTipTimeout();
	void toolTipRestart();
	void toolTipHide(bool waitForAnother = true);

protected:
	virtual void contextMenuEvent(QContextMenuEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);

	virtual void wheelEvent(QWheelEvent *event);
	virtual void leaveEvent(QEvent *event);

	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);

protected slots:
	virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous);

public:
	ContactsListWidget(KaduMainWindow *mainWindow, QWidget *parent = 0);
	virtual ~ContactsListWidget();

	ContactList selectedContacts() const;

signals:
	void contactActivated(Contact contact);
	void currentContactChanged(Contact contact);

};

#endif // CONTACTS_LIST_WIDGET_H
