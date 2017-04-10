/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "configuration/configuration-aware-object.h"
#include "exports.h"
#include "icons/kadu-icon.h"

#include <QtCore/QMultiMap>
#include <QtCore/QPointer>
#include <QtWidgets/QAction>
#include <injeqt/injeqt.h>

class ActionContext;
class ActionDescription;
class Buddy;
class BuddySet;
class Chat;
class Contact;
class ContactSet;
class IconsManager;
class MainWindow;
class StatusContainer;

/**
 * @addtogroup Actions
 * @{
 */

/**
 * @class Action
 * @short QAction extension for Kadu.
 *
 * This class is a QAction extendsion for Kadu. It contains additional methods for reveiving data about
 * contacts, buddies, chats and status containers for current action invocation. It also has methods
 * for using KaduIcons as icons instead of standard QIcons.
 */
class KADUAPI Action : public QAction
{
    Q_OBJECT

public:
    /**
     * @short Creates new Action instance based on ActionDescription with given ActionContext.
     * @param description description of this action
     * @param context data source of this action
     * @param parent parent of this action
     *
     * This method creates new instance of Action class. Action is based on description provided as
     * ActionDescription class (it stored shortcuts, icons, titles and many more information). This
     * instance of Action will use provided ActionContext instance to get information about Kadu
     * object like contacts, buddies, chats and status containers that are required to properly
     * execute each action invocation.
     *
     * Provided ActionDescription and ActionContext must not be null.
     */
    Action(ActionDescription *description, ActionContext *context, QObject *parent);
    virtual ~Action();

    /**
     * @short Returns ActionContext instance for current invocation.
     *
     * Returns ActionContext instance for current invocation. Never returns null.
     */
    ActionContext *context();

public slots:
    /**
     * @short Calls updateActionState method from ActionDescription to check if this action should be enabled or not.
     *
     * Calls updateActionState method from ActionDescription to check if this action should be enabled or not.
     */
    void checkState();

    /**
     * @short Resets icon from ActionDescription.
     *
     * Resets icon from ActionDescription. Call this slot when icon set was updated.
     */
    void updateIcon();

    /**
     * @short Sets new icon for this instance of Action.
     * @param icon new icon
     *
     * Sets new icon for this instance of Action. Used for example by StatusChange actions.
     */
    void setIcon(const KaduIcon &icon);

signals:
    /**
     * @short Emited from destructor.
     *
     * Emited from destructor. Use with great care.
     */
    void aboutToBeDestroyed(Action *action);

    /**
     * @short Emited every time action has changed (status, title, icon).
     * @param QAction this instance
     *
     * Emited every time action has changed (status, title, icon).
     */
    void changed(QAction *action);

    /**
     * @short Emited every time action is hovered.
     * @param QAction this instance
     *
     * Emited every time action is hovered.
     */
    void hovered(QAction *action);

    /**
     * @short Emited every time action is toggled.
     * @param QAction this instance
     * @param checked true if action is toggled
     *
     * Emited every time action is toggled.
     */
    void toggled(QAction *action, bool checked);

    /**
     * @short Emited every time action is triggered.
     * @param QAction this instance
     * @param checked true if action is toggled
     *
     * Emited every time action is triggered. ActionDescription class uses this to call its own virtual
     * protected method.
     */
    void triggered(QAction *action, bool checked = false);

private:
    QPointer<IconsManager> m_iconsManager;

    ActionDescription *Description;
    ActionContext *Context;

private slots:
    INJEQT_SET void setIconsManager(IconsManager *iconsManager);
    INJEQT_INIT void init();

    void changedSlot();
    void hoveredSlot();
    void triggeredSlot(bool checked);
};

void disableEmptyContacts(Action *action);
void disableNoChat(Action *action);

/**
 * @}
 */
