/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "injeqt-type-roles.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QTranslator>
#include <injeqt/injeqt.h>

class Configuration;
class PathsProvider;

class TranslationLoader : public QObject
{
    Q_OBJECT
    INJEQT_TYPE_ROLE(STARTUP)

public:
    Q_INVOKABLE explicit TranslationLoader(QObject *parent = nullptr);
    virtual ~TranslationLoader();

private:
    QPointer<Configuration> m_configuration;
    QPointer<PathsProvider> m_pathsProvider;

    QTranslator m_qt;
    QTranslator m_qtbase;
    QTranslator m_qtdeclarative;
    QTranslator m_qtmultimedia;
    QTranslator m_qtscript;
    QTranslator m_qtxmlpatterns;
    QTranslator m_kadu;

private slots:
    INJEQT_SET void setConfiguration(Configuration *configuration);
    INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);
    INJEQT_INIT void init();
    INJEQT_DONE void done();
};
