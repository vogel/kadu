/*
 * %kadu copyright begin%
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "themes/theme.h"

#include <QtCore/QHash>
#include <QtCore/QPointer>
#include <QtGui/QIcon>
#include <injeqt/injeqt.h>

class AccountManager;
class Configuration;
class IconThemeManager;
class KaduIcon;

class KADUAPI IconsManager : public QObject, public ConfigurationAwareObject
{
    Q_OBJECT

public:
    enum AllowEmpty
    {
        EmptyNotAllowed,
        EmptyAllowed
    };

    Q_INVOKABLE explicit IconsManager(QObject *parent = nullptr);
    virtual ~IconsManager();

    QString iconPath(const KaduIcon &icon, AllowEmpty allowEmpty = EmptyNotAllowed) const;
    QIcon iconByPath(const QString &themePath, const QString &path, AllowEmpty allowEmpty = EmptyNotAllowed);
    QIcon iconByPath(const KaduIcon &icon);

    QSize getIconsSize();

signals:
    void themeChanged();

protected:
    virtual void configurationUpdated();

private:
    QPointer<AccountManager> m_accountManager;
    QPointer<Configuration> m_configuration;
    QPointer<IconThemeManager> m_iconThemeManager;

    QHash<QString, QIcon> IconCache;

    void clearCache();

    QString localProtocolPath;

    QIcon buildPngIcon(const QString &themePath, const QString &path);

private slots:
    INJEQT_SET void setAccountManager(AccountManager *accountManager);
    INJEQT_SET void setConfiguration(Configuration *configuration);
    INJEQT_SET void setIconThemeManager(IconThemeManager *iconThemeManager);
    INJEQT_INIT void init();
};
