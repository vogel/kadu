/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtGui/QToolButton>

#include "configuration/configuration-aware-object.h"
#include "icons/kadu-icon.h"

class QAction;
class QMenu;

class Action;
class ActionDataSource;

class KADUAPI ActionDescription : public QObject, protected ConfigurationAwareObject
{
	Q_OBJECT

public:

	// TODO 0.10.0: this sux, but will be better
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
	KaduIcon Icon;
	QString Text;
	bool Checkable;
	ActionBoolCallback EnableCallback;
	QString ShortcutItem;
	Qt::ShortcutContext ShortcutContext;

private slots:
	void actionAboutToBeDestroyed(Action *action);
	void actionTriggeredSlot(QAction *sender, bool toggled);

protected:
	ActionDescription(QObject *parent);

	void registerAction();
	void unregisterAction();

	// TODO: rename to actionCreated after actionCreated slot is removed
	virtual void actionInstanceCreated(Action *action);

	// TODO: make abstract when actions are moved to new API
	virtual void actionTriggered(QAction *sender, bool toggled)
	{
		Q_UNUSED(sender)
		Q_UNUSED(toggled)
	}

	virtual QMenu * menuForAction(Action *action);

	virtual void connectNotify(const char *signal);
	virtual void configurationUpdated();

public:
	ActionDescription(QObject *parent, ActionType type, const QString &name, QObject *object, const char *slot,
			const KaduIcon &icon, const QString &text, bool checkable = false, ActionBoolCallback enableCallback = 0);
	virtual ~ActionDescription();

	Action * createAction(ActionDataSource *dataSource, QObject *parent);
	QList<Action *> actions();
	Action * action(ActionDataSource *dataSource);

	void setType(ActionType type);
	void setName(const QString &name);
	void setConnection(QObject *object, const char *slot);
	void setIcon(const KaduIcon &icon);
	void setText(const QString &text);
	void setCheckable(bool checkable);
	void setActionCallback(ActionBoolCallback enableCallback);

	ActionType type() const { return Type; }
	const QString & name() const { return Name; }
	const KaduIcon & icon() const { return Icon; }
	const QString & text() const { return Text; }
	bool isCheckable() const { return Checkable; }

	void setShortcut(QString configItem, Qt::ShortcutContext context = Qt::WindowShortcut);

	virtual QToolButton::ToolButtonPopupMode buttonPopupMode()
	{
		return QToolButton::MenuButtonPopup;
	}

signals:
	void actionCreated(Action *);

};

#endif // ACTION_DESCRIPTION_H
