/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "html/normalized-html-string.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QStringList>
#include <injeqt/injeqt.h>

class Configuration;
class PathsProvider;

class CenzorConfiguration : public QObject, private ConfigurationAwareObject
{
    Q_OBJECT

public:
    static QStringList toStringList(const QList<QRegExp> &list);
    static QList<QRegExp> toRegExpList(const QStringList &list);

    Q_INVOKABLE explicit CenzorConfiguration(QObject *parent = nullptr);
    virtual ~CenzorConfiguration();

    void saveConfiguration();

    bool enabled()
    {
        return Enabled;
    }
    const NormalizedHtmlString &admonition()
    {
        return Admonition;
    }
    const QList<QRegExp> &swearList()
    {
        return SwearList;
    }
    const QList<QRegExp> &exclusionList()
    {
        return ExclusionList;
    }

    void setSwearList(const QList<QRegExp> &swearList);
    void setExclusionList(const QList<QRegExp> &exclusionList);

protected:
    virtual void configurationUpdated();

private:
    QPointer<Configuration> m_configuration;
    QPointer<PathsProvider> m_pathsProvider;

    bool Enabled;
    NormalizedHtmlString Admonition;
    QList<QRegExp> SwearList;
    QList<QRegExp> ExclusionList;

    QList<QRegExp> loadRegExpList(const QString &itemName, const QString &fileName);

private slots:
    INJEQT_SET void setConfiguration(Configuration *configuration);
    INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);
    INJEQT_INIT void init();
};
