/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ACTION_DESCRIPTION_H
#define ACTION_DESCRIPTION_H

#include <QtCore/QMap>
#include <QtCore/QObject>

#include "configuration/configuration-aware-object.h"

class Action;
class KaduMainWindow;

class KADUAPI ActionDescription : public QObject, ConfigurationAwareObject
{
	Q_OBJECT

	friend class Action;

	QMap<KaduMainWindow *, Action *> MappedActions;
	int deleted;

public:

	// TODO 0.6.7: this sux, but will be better
	typedef void (*ActionBoolCallback)(Action *);

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
	const char *Slot;
	QString IconName;
	QString Text;
	QString CheckedText;
	QString ShortcutItem;
	Qt::ShortcutContext ShortcutContext;
	bool Checkable;
	ActionBoolCallback EnableCallback;

private slots:
	void actionDestroyed(QObject *action);

protected:
	virtual void configurationUpdated();

public:
	ActionDescription(QObject *parent, ActionType Type, const QString &Name, QObject *Object, const char *Slot,
		const QString &IconName, const QString &Text, bool Checkable = false, const QString &CheckedText = "", ActionBoolCallback enableCallback = 0);
	virtual ~ActionDescription();

	QString name() { return Name; }
	Action * createAction(KaduMainWindow *kaduMainWindow);
	QList<Action *> actions();
	Action * action(KaduMainWindow *kaduMainWindow);

	QString text() { return Text; }
	QString iconName() { return IconName; }

	void setShortcut(QString configItem, Qt::ShortcutContext context = Qt::WindowShortcut);

	ActionType type() { return Type; }

signals:
	void actionCreated(Action *);

};

#endif // ACTION_DESCRIPTION_H
