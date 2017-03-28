/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "os/generic/desktop-aware-object.h"

#include <QtCore/QPointer>
#include <QtWidgets/QDialog>
#include <injeqt/injeqt.h>

class IconsManager;

class QCheckBox;

class PluginErrorDialog : public QDialog, DesktopAwareObject
{
    Q_OBJECT

public:
    PluginErrorDialog(QString pluginName, const QString &text, bool offerLoadInFutureChoice, QWidget *parent = nullptr);
    virtual ~PluginErrorDialog();

public slots:
    void accept();

signals:
    void accepted(const QString &pluginName, bool loadInFuture);

private:
    QPointer<IconsManager> m_iconsManager;

    QString m_pluginName;
    QString m_text;
    bool m_offerLoadInFutureChoice;
    QCheckBox *LoadInFutureCheck;

private slots:
    INJEQT_SET void setIconsManager(IconsManager *iconsManager);
    INJEQT_INIT void init();
};
