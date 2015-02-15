/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011, 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef BUDDY_DATA_WINDOW_H
#define BUDDY_DATA_WINDOW_H

#include <QtCore/QMap>
#include <QtWidgets/QWidget>

#include "buddies/buddy.h"
#include "gui/widgets/buddy-options-configuration-widget.h"
#include "gui/widgets/configuration-value-state-notifier.h"

#include "exports.h"

class QCheckBox;
class QHostInfo;
class QLabel;
class QLineEdit;
class QComboBox;
class QPushButton;
class QScrollArea;
class QTabWidget;
class QVBoxLayout;

class Buddy;
class BuddyConfigurationWidget;
class BuddyConfigurationWidgetFactory;
class BuddyConfigurationWidgetFactoryRepository;
class BuddyGeneralConfigurationWidget;
class BuddyGroupsConfigurationWidget;
class BuddyPersonalInfoConfigurationWidget;
class CompositeConfigurationValueStateNotifier;

class KADUAPI BuddyDataWindow : public QWidget
{
	Q_OBJECT

	BuddyConfigurationWidgetFactoryRepository *MyBuddyConfigurationWidgetFactoryRepository;
	QMap<BuddyConfigurationWidgetFactory *, BuddyConfigurationWidget *> BuddyConfigurationWidgets;

	Buddy MyBuddy;
	BuddyGeneralConfigurationWidget *ContactTab;
	BuddyGroupsConfigurationWidget *GroupsTab;
	BuddyPersonalInfoConfigurationWidget *PersonalInfoTab;
	BuddyOptionsConfigurationWidget *OptionsTab;

	CompositeConfigurationValueStateNotifier *ValueStateNotifier;

	QTabWidget *TabWidget;
	QPushButton *OkButton;
	QPushButton *ApplyButton;
	QPushButton *CancelButton;

	void createGui();
	void createTabs(QLayout *layout);
	void createGeneralTab(QTabWidget *tabWidget);
	void createGroupsTab(QTabWidget *tabWidget);
	void createPersonalInfoTab(QTabWidget *tabWidget);
	void createOptionsTab(QTabWidget *tabWidget);
	void createButtons(QLayout *layout);

	void applyBuddyConfigurationWidgets();

private slots:
	void factoryRegistered(BuddyConfigurationWidgetFactory *factory);
	void factoryUnregistered(BuddyConfigurationWidgetFactory *factory);

	void stateChangedSlot(ConfigurationValueState state);

	void updateBuddy();
	void updateBuddyAndClose();
	void buddyRemoved(const Buddy &buddy);

protected:
	virtual void keyPressEvent(QKeyEvent *event);

public:
	explicit BuddyDataWindow(BuddyConfigurationWidgetFactoryRepository *buddyConfigurationWidgetFactoryRepository, const Buddy &buddy);
	virtual ~BuddyDataWindow();

	QList<BuddyConfigurationWidget *> buddyConfigurationWidgets() const;

	void show();

	Buddy buddy() const { return MyBuddy; }
	QTabWidget * tabWidget() const { return TabWidget; }
	QWidget * optionsTab() const { return OptionsTab; }

signals:
	void widgetAdded(BuddyConfigurationWidget *widget);
	void widgetRemoved(BuddyConfigurationWidget *widget);

	void destroyed(const Buddy &buddy);

};

#endif // BUDDY_DATA_WINDOW_H
