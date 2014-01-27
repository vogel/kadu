/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2002, 2004 Tomasz Jarzynka (tomee@cpi.pl)
 * Copyright 2004, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2004 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2003, 2004 Dariusz Jagodzik (mast3r@kadu.net)
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

#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QIntValidator>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <QtGui/QScrollBar>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtNetwork/QHostInfo>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "buddies/buddy-manager.h"
#include "buddies/group-manager.h"
#include "buddies/group.h"
#include "configuration/config-file-variant-wrapper.h"
#include "configuration/configuration-file.h"
#include "configuration/xml-configuration-file.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "gui/widgets/buddy-configuration-widget-factory-repository.h"
#include "gui/widgets/buddy-configuration-widget-factory.h"
#include "gui/widgets/buddy-configuration-widget-tab-adapter.h"
#include "gui/widgets/buddy-configuration-widget.h"
#include "gui/widgets/buddy-general-configuration-widget.h"
#include "gui/widgets/buddy-groups-configuration-widget.h"
#include "gui/widgets/buddy-options-configuration-widget.h"
#include "gui/widgets/buddy-personal-info-configuration-widget.h"
#include "gui/widgets/composite-configuration-value-state-notifier.h"
#include "gui/windows/message-dialog.h"
#include "misc/change-notifier.h"
#include "misc/change-notifier-lock.h"
#include "os/generic/window-geometry-manager.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol.h"
#include "activate.h"

#include "icons/icons-manager.h"
#include "debug.h"

#include "buddy-data-window.h"

BuddyDataWindow::BuddyDataWindow(BuddyConfigurationWidgetFactoryRepository *buddyConfigurationWidgetFactoryRepository, const Buddy &buddy) :
		QWidget(0, Qt::Dialog), MyBuddyConfigurationWidgetFactoryRepository(buddyConfigurationWidgetFactoryRepository), MyBuddy(buddy),
		ValueStateNotifier(new CompositeConfigurationValueStateNotifier(this))
{
	Q_ASSERT(MyBuddy != Core::instance()->myself());

	kdebugf();

	setWindowRole("kadu-buddy-data");
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Buddy Properties - %1").arg(MyBuddy.display()));

	createGui();

	new WindowGeometryManager(new ConfigFileVariantWrapper("General", "ManageUsersDialogGeometry"), QRect(0, 50, 425, 500), this);

	connect(BuddyManager::instance(), SIGNAL(buddyRemoved(Buddy)),
			this, SLOT(buddyRemoved(Buddy)));

	connect(ValueStateNotifier, SIGNAL(stateChanged(ConfigurationValueState)), this, SLOT(stateChangedSlot(ConfigurationValueState)));
	stateChangedSlot(ValueStateNotifier->state());

	if (MyBuddyConfigurationWidgetFactoryRepository)
	{
		connect(MyBuddyConfigurationWidgetFactoryRepository, SIGNAL(factoryRegistered(BuddyConfigurationWidgetFactory*)),
				this, SLOT(factoryRegistered(BuddyConfigurationWidgetFactory*)));
		connect(MyBuddyConfigurationWidgetFactoryRepository, SIGNAL(factoryUnregistered(BuddyConfigurationWidgetFactory*)),
				this, SLOT(factoryUnregistered(BuddyConfigurationWidgetFactory*)));

		foreach (BuddyConfigurationWidgetFactory *factory, MyBuddyConfigurationWidgetFactoryRepository->factories())
			factoryRegistered(factory);
	}
}

BuddyDataWindow::~BuddyDataWindow()
{
	kdebugf();
	emit destroyed(MyBuddy);
	kdebugf2();
}

void BuddyDataWindow::factoryRegistered(BuddyConfigurationWidgetFactory *factory)
{
	BuddyConfigurationWidget *widget = factory->createWidget(buddy(), this);
	if (widget)
	{
		if (widget->stateNotifier())
			ValueStateNotifier->addConfigurationValueStateNotifier(widget->stateNotifier());
		BuddyConfigurationWidgets.insert(factory, widget);
		emit widgetAdded(widget);
	}
}

void BuddyDataWindow::factoryUnregistered(BuddyConfigurationWidgetFactory *factory)
{
	if (BuddyConfigurationWidgets.contains(factory))
	{
		BuddyConfigurationWidget *widget = BuddyConfigurationWidgets.value(factory);
		if (widget->stateNotifier())
			ValueStateNotifier->removeConfigurationValueStateNotifier(widget->stateNotifier());
		emit widgetRemoved(widget);
		widget->deleteLater();
		BuddyConfigurationWidgets.remove(factory);
	}
}

QList<BuddyConfigurationWidget *> BuddyDataWindow::buddyConfigurationWidgets() const
{
	return BuddyConfigurationWidgets.values();
}

void BuddyDataWindow::applyBuddyConfigurationWidgets()
{
	foreach (BuddyConfigurationWidget *widget, BuddyConfigurationWidgets)
		widget->apply();
}

void BuddyDataWindow::show()
{
	QWidget::show();

	_activateWindow(this);
}

void BuddyDataWindow::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	createTabs(layout);
	createButtons(layout);
}

void BuddyDataWindow::createTabs(QLayout *layout)
{
	TabWidget = new QTabWidget(this);

	new BuddyConfigurationWidgetTabAdapter(this, TabWidget, this);

	createGeneralTab(TabWidget);
	createGroupsTab(TabWidget);
	createPersonalInfoTab(TabWidget);
	createOptionsTab(TabWidget);
	layout->addWidget(TabWidget);
}

void BuddyDataWindow::createGeneralTab(QTabWidget *tabWidget)
{
	ContactTab = new BuddyGeneralConfigurationWidget(MyBuddy, this);
	ValueStateNotifier->addConfigurationValueStateNotifier(ContactTab->valueStateNotifier());

	tabWidget->addTab(ContactTab, tr("General"));
}

void BuddyDataWindow::createGroupsTab(QTabWidget *tabWidget)
{
	GroupsTab = new BuddyGroupsConfigurationWidget(MyBuddy, this);
	tabWidget->addTab(GroupsTab, tr("Groups"));
}

void BuddyDataWindow::createPersonalInfoTab(QTabWidget *tabWidget)
{
	PersonalInfoTab = new BuddyPersonalInfoConfigurationWidget(MyBuddy, this);
	tabWidget->addTab(PersonalInfoTab, tr("Personal Information"));
}

void BuddyDataWindow::createOptionsTab(QTabWidget *tabWidget)
{
	OptionsTab = new BuddyOptionsConfigurationWidget(MyBuddy, this);
	tabWidget->addTab(OptionsTab, tr("Options"));
}

void BuddyDataWindow::createButtons(QLayout *layout)
{
	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);

	OkButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("OK"), this);
	buttons->addButton(OkButton, QDialogButtonBox::AcceptRole);
	ApplyButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Apply"), this);
	buttons->addButton(ApplyButton, QDialogButtonBox::ApplyRole);

	CancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);
	buttons->addButton(CancelButton, QDialogButtonBox::RejectRole);

	connect(OkButton, SIGNAL(clicked(bool)), this, SLOT(updateBuddyAndClose()));
	connect(ApplyButton, SIGNAL(clicked(bool)), this, SLOT(updateBuddy()));
	connect(CancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	layout->addWidget(buttons);
}

void BuddyDataWindow::updateBuddy()
{
	if (ValueStateNotifier->state() == StateChangedDataInvalid)
		return;

	if (!MyBuddy)
		return;

	ChangeNotifierLock lock(MyBuddy.changeNotifier());

	applyBuddyConfigurationWidgets();

	ContactTab->save();
	GroupsTab->save();
	OptionsTab->save();
}

void BuddyDataWindow::updateBuddyAndClose()
{
	if (ValueStateNotifier->state() == StateChangedDataInvalid)
		return;

	updateBuddy();
	close();
}

void BuddyDataWindow::buddyRemoved(const Buddy &buddy)
{
	if (buddy == MyBuddy)
		close();
}

void BuddyDataWindow::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape)
	{
		event->accept();
		close();
	}
	else
		QWidget::keyPressEvent(event);
}

void BuddyDataWindow::stateChangedSlot(ConfigurationValueState state)
{
	OkButton->setEnabled(state == StateChangedDataValid);
	ApplyButton->setEnabled(state == StateChangedDataValid);
	CancelButton->setEnabled(state != StateNotChanged);
}

#include "moc_buddy-data-window.cpp"
