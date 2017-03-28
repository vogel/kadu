/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "icons/kadu-icon.h"
#include "status/status.h"

#include <QtCore/QPointer>
#include <QtWidgets/QToolButton>
#include <injeqt/injeqt.h>

class IconsManager;
class InjectedFactory;
class StatusConfigurationHolder;
class StatusContainer;
class StatusIcon;
class StatusMenu;
class StatusTypeManager;

class StatusButton : public QToolButton, private ConfigurationAwareObject
{
    Q_OBJECT

public:
    explicit StatusButton(StatusContainer *statusContainer, QWidget *parent = nullptr);
    virtual ~StatusButton();

    void setDisplayStatusName(bool displayStatusName);
    bool eventFilter(QObject *object, QEvent *event);

protected:
    virtual void configurationUpdated();

private:
    QPointer<IconsManager> m_iconsManager;
    QPointer<InjectedFactory> m_injectedFactory;
    QPointer<StatusConfigurationHolder> m_statusConfigurationHolder;
    QPointer<StatusTypeManager> m_statusTypeManager;

    StatusContainer *MyStatusContainer;
    bool DisplayStatusName;
    StatusIcon *Icon;
    QAction *MenuTitleAction;

    void createGui();
    void updateStatus();

    QString prepareDescription(const QString &description) const;

private slots:
    INJEQT_SET void setIconsManager(IconsManager *iconsManager);
    INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
    INJEQT_SET void setStatusConfigurationHolder(StatusConfigurationHolder *statusConfigurationHolder);
    INJEQT_SET void setStatusTypeManager(StatusTypeManager *statusTypeManager);
    INJEQT_INIT void init();

    void iconUpdated(const KaduIcon &icon);
    void statusUpdated(StatusContainer *container = 0);
    void addTitleToMenu(const QString &title, QMenu *menu);
};
