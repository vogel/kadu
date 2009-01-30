/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KADU_MAIN_WINDOW_H
#define KADU_MAIN_WINDOW_H

#include <QtGui/QMainWindow>
#include <QtXml/QDomElement>

#include "action.h"
#include "userlist.h"

#include "exports.h"

class QContextMenuEvent;

class ContactsListWidget;

class KADUAPI KaduMainWindow : public QMainWindow
{
	Q_OBJECT

	friend class Actions;

protected:
	void loadToolBarsFromConfig(const QString &prefix);
	bool loadToolBarsFromConfig(const QString &configName, Qt::ToolBarArea area, bool remove = false);

	void writeToolBarsToConfig(const QString &prefix);
	void writeToolBarsToConfig(QDomElement parentConfig, const QString &configName, Qt::ToolBarArea area);

	static QDomElement getToolbarsConfigElement();
	static QDomElement getDockAreaConfigElement(QDomElement toolbarsConfig, const QString &name);
	static void addToolButton(QDomElement toolbarConfig, const QString &actionName, bool showLabel = false);
	static QDomElement findExistingToolbarOnArea(const QString &areaName);
	static QDomElement findExistingToolbar(const QString &prefix);

	void refreshToolBars(const QString &prefix);

	void contextMenuEvent(QContextMenuEvent *event);

public:
	KaduMainWindow(QWidget *parent);
	virtual ~KaduMainWindow();

	virtual QMenu * createPopupMenu() { return 0; }

	virtual bool supportsActionType(ActionDescription::ActionType type) = 0;
	virtual ContactsListWidget * contactsListWidget() = 0;
	virtual ContactList contacts() = 0;
	Contact contact();

	void actionAdded(KaduAction *action);

public slots:
	void addTopToolbar();
	void addBottomToolbar();
	void addLeftToolbar();
	void addRightToolbar();
	
};

#endif // KADU_MAIN_WINDOW_H
