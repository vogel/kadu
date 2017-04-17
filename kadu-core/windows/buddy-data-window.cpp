/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtGui/QIntValidator>
#include <QtGui/QKeyEvent>
#include <QtNetwork/QHostInfo>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "activate.h"
#include "buddies/buddy-manager.h"
#include "buddies/group-manager.h"
#include "buddies/group.h"
#include "configuration/config-file-variant-wrapper.h"
#include "configuration/configuration-api.h"
#include "configuration/configuration.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact.h"
#include "core/injected-factory.h"
#include "core/myself.h"
#include "misc/change-notifier-lock.h"
#include "misc/change-notifier.h"
#include "os/generic/window-geometry-manager.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol.h"
#include "widgets/buddy-configuration-widget-factory-repository.h"
#include "widgets/buddy-configuration-widget-factory.h"
#include "widgets/buddy-configuration-widget-group-boxes-adapter.h"
#include "widgets/buddy-configuration-widget.h"
#include "widgets/buddy-general-configuration-widget.h"
#include "widgets/buddy-groups-configuration-widget.h"
#include "widgets/buddy-options-configuration-widget.h"
#include "widgets/buddy-personal-info-configuration-widget.h"
#include "widgets/composite-configuration-value-state-notifier.h"
#include "windows/message-dialog.h"

#include "icons/icons-manager.h"

#include "buddy-data-window.h"
#include "buddy-data-window.moc"

BuddyDataWindow::BuddyDataWindow(const Buddy &buddy)
        : QWidget(0, Qt::Dialog), MyBuddy(buddy), ValueStateNotifier(new CompositeConfigurationValueStateNotifier(this))
{
}

BuddyDataWindow::~BuddyDataWindow()
{
    emit destroyed(MyBuddy);
}

void BuddyDataWindow::setBuddyConfigurationWidgetFactoryRepository(
    BuddyConfigurationWidgetFactoryRepository *buddyConfigurationWidgetFactoryRepository)
{
    m_buddyConfigurationWidgetFactoryRepository = buddyConfigurationWidgetFactoryRepository;
}

void BuddyDataWindow::setBuddyManager(BuddyManager *buddyManager)
{
    m_buddyManager = buddyManager;
}

void BuddyDataWindow::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void BuddyDataWindow::setInjectedFactory(InjectedFactory *injectedFactory)
{
    m_injectedFactory = injectedFactory;
}

void BuddyDataWindow::setMyself(Myself *myself)
{
    m_myself = myself;
}

void BuddyDataWindow::init()
{
    Q_ASSERT(MyBuddy != m_myself->buddy());

    setWindowRole("kadu-buddy-data");
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Buddy Properties - %1").arg(MyBuddy.display()));

    createGui();

    new WindowGeometryManager(
        new ConfigFileVariantWrapper(m_configuration, "General", "ManageUsersDialogGeometry"), QRect(0, 50, 425, 500),
        this);

    connect(m_buddyManager, SIGNAL(buddyRemoved(Buddy)), this, SLOT(buddyRemoved(Buddy)));

    connect(
        ValueStateNotifier, SIGNAL(stateChanged(ConfigurationValueState)), this,
        SLOT(stateChangedSlot(ConfigurationValueState)));
    stateChangedSlot(ValueStateNotifier->state());

    connect(
        m_buddyConfigurationWidgetFactoryRepository, SIGNAL(factoryRegistered(BuddyConfigurationWidgetFactory *)), this,
        SLOT(factoryRegistered(BuddyConfigurationWidgetFactory *)));
    connect(
        m_buddyConfigurationWidgetFactoryRepository, SIGNAL(factoryUnregistered(BuddyConfigurationWidgetFactory *)),
        this, SLOT(factoryUnregistered(BuddyConfigurationWidgetFactory *)));

    for (auto factory : m_buddyConfigurationWidgetFactoryRepository->factories())
        factoryRegistered(factory);
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
        BuddyConfigurationWidgets.remove(factory);
        if (widget)
        {
            if (widget->stateNotifier())
                ValueStateNotifier->removeConfigurationValueStateNotifier(widget->stateNotifier());
            emit widgetRemoved(widget);
            widget->deleteLater();
        }
    }
}

QList<BuddyConfigurationWidget *> BuddyDataWindow::buddyConfigurationWidgets() const
{
    return BuddyConfigurationWidgets.values();
}

void BuddyDataWindow::applyBuddyConfigurationWidgets()
{
    for (auto widget : BuddyConfigurationWidgets)
        widget->apply();
}

void BuddyDataWindow::show()
{
    QWidget::show();

    _activateWindow(m_configuration, this);
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

    createGeneralTab(TabWidget);
    createGroupsTab(TabWidget);
    createPersonalInfoTab(TabWidget);
    createOptionsTab(TabWidget);
    layout->addWidget(TabWidget);

    new BuddyConfigurationWidgetGroupBoxesAdapter(this, OptionsTab);
}

void BuddyDataWindow::createGeneralTab(QTabWidget *tabWidget)
{
    ContactTab = m_injectedFactory->makeInjected<BuddyGeneralConfigurationWidget>(MyBuddy, this);
    ValueStateNotifier->addConfigurationValueStateNotifier(ContactTab->valueStateNotifier());

    tabWidget->addTab(ContactTab, tr("General"));
}

void BuddyDataWindow::createGroupsTab(QTabWidget *tabWidget)
{
    GroupsTab = m_injectedFactory->makeInjected<BuddyGroupsConfigurationWidget>(MyBuddy, this);
    tabWidget->addTab(GroupsTab, tr("Groups"));
}

void BuddyDataWindow::createPersonalInfoTab(QTabWidget *tabWidget)
{
    PersonalInfoTab = m_injectedFactory->makeInjected<BuddyPersonalInfoConfigurationWidget>(MyBuddy, this);
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

    ContactTab->save();   // first update contacts
    GroupsTab->save();
    OptionsTab->save();

    applyBuddyConfigurationWidgets();
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
