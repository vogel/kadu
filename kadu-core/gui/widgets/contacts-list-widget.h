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

#include <QtGui/QTreeView>
#include <QtCore/QDateTime>
#include <QtCore/QTimer>

#include "contacts/contact.h"

class QContextMenuEvent;
class QModelIndex;
class QTemporaryFile;

class AbstractContactFilter;
class AbstractContactsModel;
class Contact;
class ContactSet;
class ContactsListWidgetDelegate;
class ContactsModelProxy;
class MainWindow;

class ContactsListWidget : public QTreeView


{
	Q_OBJECT

	MainWindow *MyMainWindow;
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
	QString BackgroundColor;
	BackgroundMode BackgroundImageMode;
	QString BackgroundImageFile;
	QTemporaryFile *BackgroundTemporaryFile;

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

	virtual void resizeEvent(QResizeEvent *event);

protected slots:
	virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous);
	virtual void selectionChanged( const QItemSelection &selected, const QItemSelection &deselected);

public:
	ContactsListWidget(MainWindow *mainWindow, QWidget *parent = 0);
	virtual ~ContactsListWidget();

	virtual void setModel(AbstractContactsModel *model);
	void addFilter(AbstractContactFilter *filter);
	void removeFilter(AbstractContactFilter *filter);

	Contact currentContact() const;
	ContactSet selectedContacts() const;
	void setBackground(const QString &backgroundColor, const QString &file = QString::null, BackgroundMode mode = BackgroundNone);
	void updateBackground();

signals:
	void contactActivated(Contact contact);
	void contactsSelectionChanged();

	void currentContactChanged(Contact contact);

};

#endif // CONTACTS_LIST_WIDGET_H
