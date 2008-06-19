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

#include <QDomElement>
#include <QMainWindow>

#include "action.h"
#include "userlist.h"

class QContextMenuEvent;

class KaduMainWindow : public QMainWindow
{
	Q_OBJECT

	friend class Actions;
	virtual void addAction(KaduAction *action);

protected:
	void loadToolBarsFromConfig(const QString &prefix);
	bool loadToolBarsFromConfig(const QString &configName, Qt::ToolBarArea area, bool remove = false);

	void writeToolBarsToConfig(const QString &prefix);
	void writeToolBarsToConfig(QDomElement parentConfig, const QString &configName, Qt::ToolBarArea area);

	void contextMenuEvent(QContextMenuEvent *event);

public:
	KaduMainWindow(QWidget *parent);
	virtual ~KaduMainWindow();

	virtual QMenu * createPopupMenu() { return 0; }

	virtual bool supportsActionType(ActionDescription::ActionType type) = 0;
	virtual UserBox * getUserBox() = 0;
	virtual UserListElements getUserListElements() = 0;
	virtual ChatWidget * getChatWidget() = 0;

public slots:
	void addTopToolbar();
	void addBottomToolbar();
	void addLeftToolbar();
	void addRightToolbar();
	
};

#endif // KADU_MAIN_WINDOW_H
