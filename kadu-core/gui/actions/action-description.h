/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ACTION_DESCRIPTION_H
#define ACTION_DESCRIPTION_H

#include <QtCore/QMap>
#include <QtCore/QObject>

#include "configuration/configuration-aware-object.h"

class Action;
class ActionDataSource;

class KADUAPI ActionDescription : public QObject, ConfigurationAwareObject
{
	Q_OBJECT

public:

	// TODO 0.6.7: this sux, but will be better
	typedef void (*ActionBoolCallback)(Action *);

	enum ActionType {
		TypeGlobal   = 0x0001, //!< actions with TypeGlobal type does not require access to user list or anything window-dependent
		TypeUser     = 0x0002, //!< actions with TypeUser type requires access to one or more users from user list
		TypeChat     = 0x0004, //!< actions with TypeChat type requires access to chat window
		TypeSearch   = 0x0008, //!< actions with TypeSearch type requires access to search window
		TypeUserList = 0x0010, //!< actions with TypeUserList type requires access to user list widget
		TypeHistory  = 0x0020, //!< actions with TypeHistory type requires access to history window
		TypeMainMenu = 0x0040,
		TypeAll      = 0xFFFF  //!< TypeAll is used to set masks for all types of actions
	};

private:
	friend class Action;

	QMap<ActionDataSource *, Action *> MappedActions;
	int deleted;

	ActionType Type;
	QString Name;
	QObject *Object;
	const char *Slot;
	QString IconPathOn;
	QString IconPathOff;
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
		const QString &iconPathOn, const QString &iconPathOff, const QString &Text, bool Checkable = false,
		const QString &CheckedText = QString(), ActionBoolCallback enableCallback = 0);
	virtual ~ActionDescription();

	const QString & name() const { return Name; }
	Action * createAction(ActionDataSource *dataSource, QObject *parent);
	QList<Action *> actions();
	Action * action(ActionDataSource *dataSource);

	const QString & text() const { return Text; }
	const QString & iconPathOn() const { return IconPathOn; }
	const QString & iconPathOff() const { return IconPathOff; }

	void setShortcut(QString configItem, Qt::ShortcutContext context = Qt::WindowShortcut);

	ActionType type() { return Type; }

signals:
	void actionCreated(Action *);

};

#endif // ACTION_DESCRIPTION_H
