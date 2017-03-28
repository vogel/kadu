/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#pragma once

#include "os/generic/desktop-aware-object.h"

#include <QtCore/QPointer>
#include <QtWidgets/QDialog>
#include <injeqt/injeqt.h>

class Configuration;
class UrlOpener;

class QCheckBox;

class UpdatesDialog : public QDialog, DesktopAwareObject
{
    Q_OBJECT

public:
    explicit UpdatesDialog(const QString &newestVersion, QWidget *parent = nullptr);
    virtual ~UpdatesDialog();

protected:
    virtual void keyPressEvent(QKeyEvent *e);

private:
    QPointer<Configuration> m_configuration;
    QPointer<UrlOpener> m_urlOpener;

    QString m_newestVersion;
    QCheckBox *m_checkForUpdates;

private slots:
    INJEQT_SET void setConfiguration(Configuration *configuration);
    INJEQT_SET void setUrlOpener(UrlOpener *urlOpener);
    INJEQT_INIT void init();

    void accepted();
    void downloadClicked();
};
