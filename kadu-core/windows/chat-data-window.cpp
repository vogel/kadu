/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QKeyEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

#include "activate.h"
#include "buddies/group.h"
#include "chat/chat-manager.h"
#include "chat/type/chat-type-manager.h"
#include "configuration/config-file-variant-wrapper.h"
#include "core/injected-factory.h"
#include "icons/icons-manager.h"
#include "misc/change-notifier-lock.h"
#include "misc/change-notifier.h"
#include "os/generic/window-geometry-manager.h"
#include "widgets/chat-configuration-widget-factory-repository.h"
#include "widgets/chat-configuration-widget-factory.h"
#include "widgets/chat-configuration-widget-group-boxes-adapter.h"
#include "widgets/chat-configuration-widget.h"
#include "widgets/chat-edit-widget.h"
#include "widgets/chat-groups-configuration-widget.h"
#include "widgets/composite-configuration-value-state-notifier.h"
#include "widgets/group-list.h"
#include "widgets/simple-configuration-value-state-notifier.h"

#include "chat-data-window.h"
#include "chat-data-window.moc"

ChatDataWindow::ChatDataWindow(const Chat &chat)
        : QWidget(0, Qt::Dialog), ValueStateNotifier(new CompositeConfigurationValueStateNotifier(this)),
          SimpleStateNotifier(new SimpleConfigurationValueStateNotifier(this)), MyChat(chat), EditWidget(0)
{
}

ChatDataWindow::~ChatDataWindow()
{
    emit destroyed(MyChat);
}

void ChatDataWindow::setChatConfigurationWidgetFactoryRepository(
    ChatConfigurationWidgetFactoryRepository *chatConfigurationWidgetFactoryRepository)
{
    m_chatConfigurationWidgetFactoryRepository = chatConfigurationWidgetFactoryRepository;
}

void ChatDataWindow::setChatManager(ChatManager *chatManager)
{
    m_chatManager = chatManager;
}

void ChatDataWindow::setChatTypeManager(ChatTypeManager *chatTypeManager)
{
    m_chatTypeManager = chatTypeManager;
}

void ChatDataWindow::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void ChatDataWindow::setInjectedFactory(InjectedFactory *injectedFactory)
{
    m_injectedFactory = injectedFactory;
}

void ChatDataWindow::init()
{
    setWindowRole("kadu-chat-data");
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Chat Properties - %1").arg(MyChat.display()));

    createGui();

    new WindowGeometryManager(
        new ConfigFileVariantWrapper(m_configuration, "General", "ChatDataWindowGeometry"), QRect(0, 50, 425, 500),
        this);

    connect(m_chatManager, SIGNAL(chatRemoved(Chat)), this, SLOT(chatRemoved(Chat)));

    SimpleStateNotifier->setState(StateNotChanged);
    ValueStateNotifier->addConfigurationValueStateNotifier(SimpleStateNotifier);

    connect(
        ValueStateNotifier, SIGNAL(stateChanged(ConfigurationValueState)), this,
        SLOT(stateChangedSlot(ConfigurationValueState)));
    stateChangedSlot(ValueStateNotifier->state());

    connect(
        m_chatConfigurationWidgetFactoryRepository, SIGNAL(factoryRegistered(ChatConfigurationWidgetFactory *)), this,
        SLOT(factoryRegistered(ChatConfigurationWidgetFactory *)));
    connect(
        m_chatConfigurationWidgetFactoryRepository, SIGNAL(factoryUnregistered(ChatConfigurationWidgetFactory *)), this,
        SLOT(factoryUnregistered(ChatConfigurationWidgetFactory *)));

    for (auto factory : m_chatConfigurationWidgetFactoryRepository->factories())
        factoryRegistered(factory);
}

void ChatDataWindow::factoryRegistered(ChatConfigurationWidgetFactory *factory)
{
    ChatConfigurationWidget *widget = factory->createWidget(chat(), this);
    if (widget)
    {
        if (widget->stateNotifier())
            ValueStateNotifier->addConfigurationValueStateNotifier(widget->stateNotifier());
        ChatConfigurationWidgets.insert(factory, widget);
        emit widgetAdded(widget);
    }
}

void ChatDataWindow::factoryUnregistered(ChatConfigurationWidgetFactory *factory)
{
    if (ChatConfigurationWidgets.contains(factory))
    {
        ChatConfigurationWidget *widget = ChatConfigurationWidgets.value(factory);
        ChatConfigurationWidgets.remove(factory);
        if (widget)
        {
            if (widget->stateNotifier())
                ValueStateNotifier->removeConfigurationValueStateNotifier(widget->stateNotifier());
            emit widgetRemoved(widget);
            widget->deleteLater();
        }
    }
}

QList<ChatConfigurationWidget *> ChatDataWindow::chatConfigurationWidgets() const
{
    return ChatConfigurationWidgets.values();
}

void ChatDataWindow::applyChatConfigurationWidgets()
{
    for (auto widget : ChatConfigurationWidgets)
        widget->apply();
}

void ChatDataWindow::show()
{
    QWidget::show();

    _activateWindow(m_configuration, this);
}

void ChatDataWindow::createGui()
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    TabWidget = new QTabWidget(this);

    GeneralTab = new QWidget(TabWidget);
    QVBoxLayout *generalLayout = new QVBoxLayout(GeneralTab);

    QWidget *nameWidget = new QWidget(this);

    QHBoxLayout *nameLayout = new QHBoxLayout(nameWidget);

    QLabel *numberLabel = new QLabel(tr("Visible Name") + ':', nameWidget);

    DisplayEdit = new QLineEdit(nameWidget);
    DisplayEdit->setText(MyChat.display());

    nameLayout->addWidget(numberLabel);
    nameLayout->addWidget(DisplayEdit);

    generalLayout->addWidget(nameWidget);

    TabWidget->addTab(GeneralTab, tr("General"));

    auto chatType = m_chatTypeManager->chatType(MyChat.type());
    if (chatType)
    {
        EditWidget = chatType->createEditWidget(MyChat, TabWidget);
        if (EditWidget)
        {
            auto groupBox = new QGroupBox{GeneralTab};
            groupBox->setFlat(true);
            groupBox->setTitle(tr("Chat"));

            auto groupBoxLayout = new QVBoxLayout{groupBox};
            groupBoxLayout->setMargin(0);
            groupBoxLayout->setSpacing(4);
            groupBoxLayout->addWidget(EditWidget);

            generalLayout->addWidget(groupBox);
            if (EditWidget->stateNotifier())
                ValueStateNotifier->addConfigurationValueStateNotifier(EditWidget->stateNotifier());
        }
    }

    generalLayout->addStretch(100);

    GroupsTab = m_injectedFactory->makeInjected<ChatGroupsConfigurationWidget>(MyChat, this);
    TabWidget->addTab(GroupsTab, tr("Groups"));

    auto optionsTab = new QWidget{this};
    (new QVBoxLayout{optionsTab})->addStretch(100);
    new ChatConfigurationWidgetGroupBoxesAdapter(this, optionsTab);
    TabWidget->addTab(optionsTab, tr("Options"));

    layout->addWidget(TabWidget);

    createButtons(layout);

    connect(DisplayEdit, SIGNAL(textChanged(QString)), this, SLOT(displayEditChanged()));
}

void ChatDataWindow::createButtons(QVBoxLayout *layout)
{
    QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);

    OkButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("OK"), this);
    OkButton->setDefault(true);
    buttons->addButton(OkButton, QDialogButtonBox::AcceptRole);
    ApplyButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Apply"), this);
    buttons->addButton(ApplyButton, QDialogButtonBox::ApplyRole);

    QPushButton *cancelButton =
        new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);
    buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

    connect(OkButton, SIGNAL(clicked(bool)), this, SLOT(updateChatAndClose()));
    connect(ApplyButton, SIGNAL(clicked(bool)), this, SLOT(updateChat()));
    connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));

    layout->addSpacing(16);
    layout->addWidget(buttons);
}

void ChatDataWindow::updateChat()
{
    if (!MyChat)
        return;

    ChangeNotifierLock lock(MyChat.changeNotifier());

    if (EditWidget)
        EditWidget->apply();

    GroupsTab->save();

    applyChatConfigurationWidgets();

    MyChat.setDisplay(DisplayEdit->text());

    emit save();
}

void ChatDataWindow::updateChatAndClose()
{
    updateChat();
    close();
}

void ChatDataWindow::chatRemoved(const Chat &chat)
{
    if (chat == MyChat)
        close();
}

void ChatDataWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        event->accept();
        close();
    }
    else
        QWidget::keyPressEvent(event);
}

void ChatDataWindow::displayEditChanged()
{
    if (MyChat.display() == DisplayEdit->text())
    {
        SimpleStateNotifier->setState(StateNotChanged);
        return;
    }

    const Chat &chat = m_chatManager->byDisplay(DisplayEdit->text());
    if (chat)
        SimpleStateNotifier->setState(StateChangedDataInvalid);
    else
        SimpleStateNotifier->setState(StateChangedDataValid);
}

void ChatDataWindow::stateChangedSlot(ConfigurationValueState state)
{
    OkButton->setEnabled(state != StateChangedDataInvalid);
    ApplyButton->setEnabled(state != StateChangedDataInvalid);
}
