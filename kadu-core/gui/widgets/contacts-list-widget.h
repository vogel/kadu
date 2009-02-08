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

class AbstractContactFilter;
class AbstractContactsModel;
class ContactList;
class ContactsListWidgetDelegate;
class ContactsModelProxy;
class KaduMainWindow;

class ContactsListWidget : public QListView
{
	Q_OBJECT

	KaduMainWindow *MainWindow;
	ContactsListWidgetDelegate *Delegate;
	ContactsModelProxy *ProxyModel;

	Contact contact(const QModelIndex &index) const;
	void triggerActivate(const QModelIndex &index);

	// Tool tips
	Contact ToolTipContact;
	QTimer ToolTipTimeoutTimer;

public:
	enum BackgroundMode
	{
		BackgroundNone,
		BackgroundCentered,
		BackgroundTiled,
		BackgroundTiledAndCentered,
		BackgroundStretched
	};

private:
	BackgroundMode BackgroundImageMode;
	QString BackgroundImageFile;

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

	virtual void setModel(AbstractContactsModel *model);
	void addFilter(AbstractContactFilter *filter);
	void removeFilter(AbstractContactFilter *filter);

	ContactList selectedContacts() const;
	void setBackground(const QString &file = QString::null, BackgroundMode mode = BackgroundNone);
	void updateBackground();

signals:
	void contactActivated(Contact contact);
	void currentContactChanged(Contact contact);

};

#endif // CONTACTS_LIST_WIDGET_H
