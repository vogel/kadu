/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "cenzor-configuration.h"
#include "cenzor-configuration.moc"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "html/html-conversion.h"
#include "html/html-string.h"
#include "misc/paths-provider.h"

#include <QtCore/QFile>
#include <QtCore/QTextStream>

QStringList CenzorConfiguration::toStringList(const QList<QRegExp> &list)
{
    QStringList result;
    for (auto const &regExp : list)
        result.append(regExp.pattern());
    return result;
}

QList<QRegExp> CenzorConfiguration::toRegExpList(const QStringList &list)
{
    QList<QRegExp> result;
    for (auto const &item : list)
        result.append(QRegExp(item));
    return result;
}

CenzorConfiguration::CenzorConfiguration(QObject *parent) : QObject{parent}
{
}

CenzorConfiguration::~CenzorConfiguration()
{
}

void CenzorConfiguration::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void CenzorConfiguration::setPathsProvider(PathsProvider *pathsProvider)
{
    m_pathsProvider = pathsProvider;
}

void CenzorConfiguration::init()
{
    configurationUpdated();
}

void CenzorConfiguration::setExclusionList(const QList<QRegExp> &exclusionList)
{
    ExclusionList = exclusionList;
}

void CenzorConfiguration::setSwearList(const QList<QRegExp> &swearList)
{
    SwearList = swearList;
}

QList<QRegExp> CenzorConfiguration::loadRegExpList(const QString &itemName, const QString &fileName)
{
    QList<QRegExp> result = toRegExpList(
        m_configuration->deprecatedApi()->readEntry("PowerKadu", itemName).split('\t', QString::SkipEmptyParts));

    if (!result.empty())
        return result;

    QFile file(fileName);

    if (!file.open(QFile::ReadOnly))
        return result;

    QTextStream stream(&file);
    while (!stream.atEnd())
        result += QRegExp(stream.readLine());
    file.close();

    return result;
}

void CenzorConfiguration::configurationUpdated()
{
    Enabled = m_configuration->deprecatedApi()->readBoolEntry("PowerKadu", "enable_cenzor");
    Admonition = normalizeHtml(
        HtmlString{m_configuration->deprecatedApi()->readEntry(
            "PowerKadu", "admonition_content_cenzor", "Cenzor: Watch your mouth!! <nonono>")});
    SwearList = loadRegExpList(
        "cenzor swearwords", m_pathsProvider->dataPath() + QStringLiteral("plugins/data/cenzor/cenzor_words.conf"));
    ExclusionList = loadRegExpList(
        "cenzor exclusions", m_pathsProvider->dataPath() + QStringLiteral("plugins/data/cenzor/cenzor_words_ok.conf"));
}

void CenzorConfiguration::saveConfiguration()
{
    m_configuration->deprecatedApi()->writeEntry("PowerKadu", "cenzor swearwords", toStringList(SwearList).join("\t"));
    m_configuration->deprecatedApi()->writeEntry(
        "PowerKadu", "cenzor exclusions", toStringList(ExclusionList).join("\t"));
}
