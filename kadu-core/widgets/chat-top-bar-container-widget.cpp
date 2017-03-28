/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtWidgets/QVBoxLayout>

#include "widgets/chat-top-bar-widget-factory-repository.h"
#include "widgets/chat-top-bar-widget-factory.h"

#include "chat-top-bar-container-widget.h"

ChatTopBarContainerWidget::ChatTopBarContainerWidget(const Chat &chat, QWidget *parent) : QWidget(parent), MyChat(chat)
{
}

ChatTopBarContainerWidget::~ChatTopBarContainerWidget()
{
}

void ChatTopBarContainerWidget::setChatTopBarWidgetFactoryRepository(
    ChatTopBarWidgetFactoryRepository *chatTopBarWidgetFactoryRepository)
{
    m_chatTopBarWidgetFactoryRepository = chatTopBarWidgetFactoryRepository;
}

void ChatTopBarContainerWidget::init()
{
    createGui();

    connect(
        m_chatTopBarWidgetFactoryRepository, SIGNAL(factoryRegistered(ChatTopBarWidgetFactory *)), this,
        SLOT(factoryRegistered(ChatTopBarWidgetFactory *)));
    connect(
        m_chatTopBarWidgetFactoryRepository, SIGNAL(factoryUnregistered(ChatTopBarWidgetFactory *)), this,
        SLOT(factoryUnregistered(ChatTopBarWidgetFactory *)));

    foreach (ChatTopBarWidgetFactory *factory, m_chatTopBarWidgetFactoryRepository->factories())
        factoryRegistered(factory);
}

void ChatTopBarContainerWidget::createGui()
{
    Layout = new QVBoxLayout(this);
    Layout->setMargin(0);
    Layout->setSpacing(0);
}

void ChatTopBarContainerWidget::factoryRegistered(ChatTopBarWidgetFactory *factory)
{
    QWidget *widget = factory->createWidget(MyChat, this);
    if (widget)
    {
        TopBarWidgets.insert(factory, widget);
        Layout->addWidget(widget);
    }
}

void ChatTopBarContainerWidget::factoryUnregistered(ChatTopBarWidgetFactory *factory)
{
    if (TopBarWidgets.contains(factory))
    {
        QWidget *widget = TopBarWidgets.value(factory);
        widget->deleteLater();
    }
}
