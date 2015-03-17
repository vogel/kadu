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

#include "buddies/group.h"
#include "chat/chat-manager.h"
#include "chat/type/chat-type-manager.h"
#include "configuration/config-file-variant-wrapper.h"
#include "gui/widgets/chat-configuration-widget-factory-repository.h"
#include "gui/widgets/chat-configuration-widget-factory.h"
#include "gui/widgets/chat-configuration-widget-group-boxes-adapter.h"
#include "gui/widgets/chat-configuration-widget.h"
#include "gui/widgets/chat-edit-widget.h"
#include "gui/widgets/chat-groups-configuration-widget.h"
#include "gui/widgets/composite-configuration-value-state-notifier.h"
#include "gui/widgets/group-list.h"
#include "gui/widgets/simple-configuration-value-state-notifier.h"
#include "icons/icons-manager.h"
#include "misc/change-notifier-lock.h"
#include "misc/change-notifier.h"
#include "os/generic/window-geometry-manager.h"
#include "activate.h"

#include "chat-data-window.h"

ChatDataWindow::ChatDataWindow(ChatConfigurationWidgetFactoryRepository *chatConfigurationWidgetFactoryRepository, const Chat &chat) :
		QWidget(0, Qt::Dialog), MyChatConfigurationWidgetFactoryRepository(chatConfigurationWidgetFactoryRepository),
		ValueStateNotifier(new CompositeConfigurationValueStateNotifier(this)),
		SimpleStateNotifier(new SimpleConfigurationValueStateNotifier(this)),
		MyChat(chat), EditWidget(0)
{
	setWindowRole("kadu-chat-data");
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Chat Properties - %1").arg(MyChat.display()));

	createGui();

	new WindowGeometryManager(new ConfigFileVariantWrapper("General", "ChatDataWindowGeometry"), QRect(0, 50, 425, 500), this);

	connect(ChatManager::instance(), SIGNAL(chatRemoved(Chat)),
			this, SLOT(chatRemoved(Chat)));

	SimpleStateNotifier->setState(StateNotChanged);
	ValueStateNotifier->addConfigurationValueStateNotifier(SimpleStateNotifier);

	connect(ValueStateNotifier, SIGNAL(stateChanged(ConfigurationValueState)), this, SLOT(stateChangedSlot(ConfigurationValueState)));
	stateChangedSlot(ValueStateNotifier->state());

	if (MyChatConfigurationWidgetFactoryRepository)
	{
		connect(MyChatConfigurationWidgetFactoryRepository, SIGNAL(factoryRegistered(ChatConfigurationWidgetFactory*)),
				this, SLOT(factoryRegistered(ChatConfigurationWidgetFactory*)));
		connect(MyChatConfigurationWidgetFactoryRepository, SIGNAL(factoryUnregistered(ChatConfigurationWidgetFactory*)),
				this, SLOT(factoryUnregistered(ChatConfigurationWidgetFactory*)));

		foreach (ChatConfigurationWidgetFactory *factory, MyChatConfigurationWidgetFactoryRepository->factories())
			factoryRegistered(factory);
	}
}

ChatDataWindow::~ChatDataWindow()
{
	emit destroyed(MyChat);
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
	foreach (ChatConfigurationWidget *widget, ChatConfigurationWidgets)
		widget->apply();
}

void ChatDataWindow::show()
{
	QWidget::show();

	_activateWindow(this);
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

	ChatType *chatType = ChatTypeManager::instance()->chatType(MyChat.type());
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

	GroupsTab = new ChatGroupsConfigurationWidget(MyChat, this);
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

	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);
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

	const Chat &chat = ChatManager::instance()->byDisplay(DisplayEdit->text());
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

#include "moc_chat-data-window.cpp"
