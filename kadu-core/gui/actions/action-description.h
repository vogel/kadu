/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
class ActionContext;

/**
 * @addtogroup Actions
 * @{
 */

/**
 * @class ActionDescription
 * @author Rafał 'Vogel' Malinowski
 * @short Class responsible for describing and creating actions for windows.
 *
 * This class is used to describe actions that can be created and inserted into menus and
 * toolbars on different types of windows. Some actions can be only used in connection with
 * chats, users, search window or history window. Some actions are only useable in main
 * menu of Kadu.
 *
 * For each action each window gets its own instance of Action class created by given ActionDescription
 * object. This is required because each window contains different objects (like chats, contacts and
 * buddies) and may have different conditions for enabling/disabling actions. Windows are mapped
 * as ActionContext interfaces - each type of window or widget can have its own implementation
 * of this interface.
 *
 * Actions can be simple actions or actions with menu. If action has menu, it has to implement
 * menuForAction method. Each menuForAction must return different instance of menu, as this method is called
 * only once for each Action and menu will be destroyed when Action is destroyed.
 *
 * ActionDescription has two constructors. One of them is public and has a lot ofparameters. This
 * is depreceated and will be removed in 0.11.0 or 0.12.0 release. This constructor registers action
 * automatically.
 *
 * Second constructor is protected and had only one parameter - parent. This one should be called
 * by subclasses. Then all setXXX methods should be called to set up proper values of ActionDescription
 * properties. After setting all values registerAction method must be called so action can be used
 * on toolbars in menus. Do not call registerAction before setting all properties.
 *
 * Unregistering of action is automatically performed by destructor.
 */
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

	QMap<ActionContext *, Action *> MappedActions;
	bool Deleting;

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
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Creates new instance of ActionDescription with given parent.
	 * @param parent QObject parent of ActionDescription.
	 *
	 * Created new empty instance of ActionDescription. Call setters and registerActions to
	 * make this action description useable.
	 */
	ActionDescription(QObject *parent);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Registers this action in global action list.
	 *
	 * Registers this action in global action list. Called automatically by depreceated public
	 * constructor. In new implementation this should be called from subclasses after all apprioprate
	 * setters were called.
	 */
	void registerAction();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Unregisters this action from global action list.
	 *
	 * Registers this action from global action list. Called automatically by destructor. No need to call
	 * this manually.
	 */
	void unregisterAction();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @todo rename to actionCreated after actionCreated slot is removed
	 * @short Called when new instance of Action is created.
	 * @param action newly created action
	 *
	 * This method is called automatically when new instance of Action is created. It allows to set uo
	 * additional Action parameters.
	 *
	 * By defult this method checks if this instance of Actions should have menu attached and creates it
	 * if needed (see menuForAction).
	 *
	 * Old implementations of actions uses actionCreated signal that is now depreceated.
	 */
	virtual void actionInstanceCreated(Action *action);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @todo make abstract when actions are moved to new API
	 * @short Called when instance of Action is triggered.
	 * @param sender instance of QAction that was triggered
	 * @param toggled true, if action was toggled on
	 *
	 * This method is called automatically when instance of QAction is triggered.
	 *
	 * By defult this method does nothing.
	 *
	 * Old implementations of actions uses object and slot parameters in constructor to get notification
	 * about action triggering.
	 */
	virtual void actionTriggered(QAction *sender, bool toggled)
	{
		Q_UNUSED(sender)
		Q_UNUSED(toggled)
	}

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @todo make abstract when actions are moved to new API
	 * @short Called when instance of Action is triggered.
	 * @param widget parent widget of triggered action
	 * @param context context of triggered action
	 * @param toggled true, if action was toggled on
	 *
	 * This method is called automatically when instance of QAction is triggered.
	 *
	 * By defult this method does nothing.
	 *
	 * Old implementations of actions uses object and slot parameters in constructor to get notification
	 * about action triggering.
	 */
	virtual void triggered(QWidget *widget, ActionContext *context, bool toggled)
	{
		Q_UNUSED(widget)
		Q_UNUSED(context)
		Q_UNUSED(toggled)
	}

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Updates enabled/disabled state of given action.
	 * @param action action to update state for
	 *
	 * This method is called by by Action class when it requies to update its own state (like disabled/enable).
	 * By defult this method calls EnableCallback callback for this action.
	 */
	virtual void updateActionState(Action *action);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Creates menu for given new instance of Action.
	 * @param action action to create menu for
	 *
	 * This method is called by default implementation of actionInstanceCreated. This method should
	 * return null when no menu for given action is required. New instance of QMenu should be returned
	 * when this actions requires menu. Please note that Action takes ownership of this QMenu instance
	 * and can delete it at any moment.
	 *
	 * By defult this method returns null.
	 */
	virtual QMenu * menuForAction(Action *action);

	virtual void connectNotify(const char *signal);
	virtual void configurationUpdated();

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Depreceated contructor.
	 *
	 * Depreceated contructor.
	 */
	ActionDescription(QObject *parent, ActionType type, const QString &name, QObject *object, const char *slot,
			const KaduIcon &icon, const QString &text, bool checkable = false, ActionBoolCallback enableCallback = 0);
	virtual ~ActionDescription();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Creates instance of Action for given ActionContext.
	 * @param context context of new Action instance
	 * @param parent parent of new Action instance
	 *
	 * This method creates new instance of Action for given ActionContext or returns existing one, if action for
	 * given ActionContext was already created. Each ActionContext will have different instance of Actions.
	 *
	 * This method calls actionInstanceCreated and emits actionCreated if new instance were created.
	 */
	Action * createAction(ActionContext *context, QObject *parent);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns list of all Actions created from this ActionDescription.
	 *
	 * Returns list of all Actions created from this ActionDescription.
	 */
	QList<Action *> actions();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns Action instance connected with given ActionContext.
	 * @param context ActionContext to search Action for
	 *
	 * Returns Action instance connected with given ActionContext or 0 when no Action was created.
	 */
	Action * action(ActionContext *context);

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

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns QToolButton::ToolButtonPopupMode for given action.
	 *
	 * Returns QToolButton::ToolButtonPopupMode for given action. This allows actions with menu that
	 * can have default behavior or not. By default actions have default behaviour - QToolButton::MenuButtonPopup
	 * is returned.
	 *
	 * If action does not have default behaviour no method will be called after clicking on method. Reimplementing
	 * actionTriggered is then not required.
	 */
	virtual QToolButton::ToolButtonPopupMode buttonPopupMode()
	{
		return QToolButton::MenuButtonPopup;
	}

signals:
	void actionCreated(Action *);

};

/**
 * @}
 */

#endif // ACTION_DESCRIPTION_H
