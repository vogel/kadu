/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddies/group.h"

#include <QtCore/QPointer>
#include <QtWidgets/QDialog>
#include <injeqt/injeqt.h>

class DeprecatedConfigurationApi;
class GroupManager;
class GroupStorage;
class IconsManager;

class QCheckBox;
class QLabel;
class QLineEdit;
class QToolButton;

class GroupEditWindow : public QDialog
{
    Q_OBJECT

public:
    explicit GroupEditWindow(Group group, QWidget *parent = nullptr);
    virtual ~GroupEditWindow();

protected:
    virtual void accept() override;

private:
    QPointer<Configuration> m_configuration;
    QPointer<GroupManager> m_groupManager;
    QPointer<GroupStorage> m_groupStorage;
    QPointer<IconsManager> m_iconsManager;

    Group m_group;
    bool m_add;
    QString m_selectedIcon;

    QLineEdit *m_nameLineEdit;
    QLabel *m_nameErrorLabel;
    QCheckBox *m_nameCheckBox;
    QCheckBox *m_iconCheckBox;
    QToolButton *m_selectIconButton;
    QCheckBox *m_notifyCheckBox;
    QCheckBox *m_offlineCheckBox;
    QCheckBox *m_allGroupCheckBox;
    QPushButton *m_saveButton;
    QPushButton *m_applyButton;

    void createGui();
    void loadValues();
    void storeValues();

private slots:
    INJEQT_SET void setConfiguration(Configuration *configuration);
    INJEQT_SET void setGroupManager(GroupManager *groupManager);
    INJEQT_SET void setGroupStorage(GroupStorage *groupStorage);
    INJEQT_SET void setIconsManager(IconsManager *iconsManager);
    INJEQT_INIT void init();

    void selectIcon();
    void dataChanged();

    void apply();
};
