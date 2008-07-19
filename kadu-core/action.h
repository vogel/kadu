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

#include <QtCore/QMultiMap>
#include <QtGui/QAction>
#include <QtGui/QIcon>

class ChatWidget;
class KaduMainWindow;
class ToolBar;
class UserBox;
class UserListElements;

class ActionDescription;

class KaduAction : public QAction
{
	Q_OBJECT

	ActionDescription *Description;

	QString OnText;
	QString OffText;

	QIcon OnIcon;
	QIcon OffIcon;

private slots:
	void changedSlot();
	void hoveredSlot();
	void triggeredSlot(bool checked);

public:
	KaduAction(ActionDescription *description, KaduMainWindow *parent);
	virtual ~KaduAction();

	UserListElements userListElements();

public slots:
	void checkIfEnabled();
	virtual void setChecked(bool checked);

signals:
	void changed(QAction *action);
	void hovered(QAction *action);
	void toggled(QAction *action, bool checked);
	void triggered(QAction *action, bool checked = false);

};

class ActionDescription : public QObject
{
	Q_OBJECT

	friend class KaduAction;

	QMultiMap<KaduMainWindow *, KaduAction *> MappedActions;

public:

	// TODO 0.6.7: this sux, but will be better
	typedef bool (*ActionBoolCallback)(KaduAction *);

	enum ActionType {
		TypeGlobal   = 0x0001, //!< actions with TypeGlobal type does not require access to user list or anything window-dependend
		TypeUser     = 0x0002, //!< actions with TypeUser type requires access to one or more users from user list
		TypeChat     = 0x0004, //!< actions with TypeChat type requires access to chat window
		TypeSearch   = 0x0008, //!< actions with TypeSearch type requires access to search window
		TypeUserList = 0x0010, //!< actions with TypeUserList type requires access to user list widget
		TypeHistory  = 0x0020, //!< actions with TypeHistory type requires access to history window
		TypeMainMenu = 0x0040, 
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
	ActionBoolCallback EnableCallback;

private slots:
	void actionDestroyed(QObject *action);

public:
	ActionDescription(ActionType Type, const QString &Name, QObject *Object, char *Slot,
		const QString &IconName, const QString &Text, bool Checkable = false, const QString &CheckedText = "", ActionBoolCallback enableCallback = 0);
	virtual ~ActionDescription();

	QString name() { return Name; }
	KaduAction * createAction(KaduMainWindow *kaduMainWindow);
	QList<KaduAction *> actions();
	QList<KaduAction *> actions(KaduMainWindow *kaduMainWindow);

	QString text() { return Text; }
	QString iconName() { return IconName; }

	ActionType type() { return Type; }

signals:
	void actionCreated(KaduAction *);

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

	QAction * createAction(const QString &name, KaduMainWindow *kaduMainWindow);
	void refreshIcons();

signals:
	void actionCreated(KaduAction *);
	void actionLoaded(const QString &actionName);
	void actionUnloaded(const QString &actionName);

};

bool disableEmptyUles(KaduAction *action);

extern Actions KaduActions;

#endif
