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

#include <QtWidgets/QDialog>

class DeprecatedConfigurationApi;
class GroupManager;

class QCheckBox;
class QLabel;
class QLineEdit;
class QToolButton;

class GroupEditWindow : public QDialog
{
	Q_OBJECT

public:
	explicit GroupEditWindow(GroupManager *groupManager, DeprecatedConfigurationApi *configuration, Group group, QWidget *parent = nullptr);
	virtual ~GroupEditWindow();

protected:
	virtual void accept() override;

private:
	GroupManager *m_groupManager;
	DeprecatedConfigurationApi *m_configuration;

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
	void selectIcon();
	void dataChanged();

	void apply();

};
