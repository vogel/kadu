/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ACTION_H
#define ACTION_H

#include <QAction>
#include <QIcon>

class ChatWidget;
class KaduMainWindow;
class ToolBar;
class UserBox;

class KaduAction : public QAction
{
	Q_OBJECT

	QString OnText;
	QString OffText;

	QIcon OnIcon;
	QIcon OffIcon;

	void connectSignalsAndSlots();

private slots:
	void changedSlot();
	void hoveredSlot();
	void toggledSlot(bool checked);
	void triggeredSlot(bool checked);

public:
	KaduAction(QObject *parent);
	KaduAction(const QString &text, QObject *parent);
	KaduAction(const QString &onText, const QString &offText, QObject *parent);
	KaduAction(const QIcon &icon, const QString &text, QObject *parent);
	KaduAction(const QIcon &onIcon, const QIcon offIcon, const QString &onText, const QString &offText, QObject *parent);
	virtual ~KaduAction();

signals:
	void changed(QAction *action);
	void hovered(QAction *action);
	void toggled(QAction *action, bool checked);
	void triggered(QAction *action, bool checked = false);

};

class ActionDescription : public QObject
{
	Q_OBJECT

public:
	enum ActionType {
		TypeGlobal   = 0x0001, //!< actions with TypeGlobal type does not require access to user list or anything window-dependend
		TypeUser     = 0x0002, //!< actions with TypeUser type requires access to one or more users from user list
		TypeChat     = 0x0004, //!< actions with TypeChat type requires access to chat window
		TypeSearch   = 0x0008, //!< actions with TypeSearch type requires access to search window
		TypeUserList = 0x0010, //!< actions with TypeUserList type requires access to user list widget
		TypeAll      = 0xFFFF  //!< TypeAll is used to set masks for all types of actions
	};

private:
	ActionType Type;
	QString Name;
	QObject *Object;
	char *Slot;
	QString IconName;
	QString Text;
	QString CheckedText;
	bool Checkable;

public:
	ActionDescription(ActionType Type, const QString &Name, QObject *Object, char *Slot,
		const QString &IconName, const QString &Text, bool Checkable = false, const QString &CheckedText = "");
	virtual ~ActionDescription();

	QString name() { return Name; }
	KaduAction *getAction(KaduMainWindow *kaduMainWindow);

	QString text() { return Text; }
	QString iconName() { return IconName; }

	ActionType type() { return Type; }

};

class Actions : public QObject, public QMap<QString, ActionDescription *>
{
	Q_OBJECT

	void insert(ActionDescription *action);
	void remove(ActionDescription *action);
	friend class ActionDescription;

public:
	Actions();
	virtual ~Actions() {}

	QAction *getAction(const QString &name, KaduMainWindow *kaduMainWindow) const;
	void refreshIcons();

signals:
	void actionLoaded(const QString &actionName);
	void actionUnloaded(const QString &actionName);

};

extern Actions KaduActions;

#endif
