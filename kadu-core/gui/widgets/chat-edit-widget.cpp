/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gui/widgets/simple-configuration-value-state-notifier.h"

#include "chat-edit-widget.h"

ChatEditWidget::ChatEditWidget(Chat chat, QWidget *parent) :
		ModalConfigurationWidget{parent},
		m_stateNotifier{make_owned<SimpleConfigurationValueStateNotifier>(this)},
		m_chat{chat}
{
}

ChatEditWidget::~ChatEditWidget()
{
}

Chat ChatEditWidget::chat() const
{
	return m_chat;
}

SimpleConfigurationValueStateNotifier * ChatEditWidget::simpleStateNotifier() const
{
	return m_stateNotifier.get();
}

const ConfigurationValueStateNotifier * ChatEditWidget::stateNotifier() const
{
	return m_stateNotifier.get();
}
