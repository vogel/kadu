/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "configuration/configuration-file.h"
#include "gui/actions/action.h"
#include "gui/widgets/chat-edit-box.h"

#include "configuration/emoticon-configuration.h"
#include "expander/animated-emoticon-path-provider.h"
#include "expander/static-emoticon-path-provider.h"
#include "gui/emoticon-selector.h"

#include "insert-emoticon-action.h"

InsertEmoticonAction::InsertEmoticonAction(QObject *parent) :
		ActionDescription(parent)
{
	setType(ActionDescription::TypeChat);
	setName("insertEmoticonAction");
	setIcon(KaduIcon("face-smile"));
	setText(tr("Insert Emoticon"));

	registerAction();
}

InsertEmoticonAction::~InsertEmoticonAction()
{
}

void InsertEmoticonAction::actionInstanceCreated(Action *action)
{
	ActionDescription::actionInstanceCreated(action);

	updateActionState(action);
}

void InsertEmoticonAction::updateActionState(Action *action)
{
	ActionDescription::updateActionState(action);

	if (Configuration.enabled())
	{
		action->setToolTip(tr("Insert emoticon"));
		action->setEnabled(true);
	}
	else
	{
		action->setToolTip(tr("Insert emoticon - enable in configuration"));
		action->setEnabled(false);
	}
}

void InsertEmoticonAction::actionTriggered(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	QList<QWidget *> widgets = sender->associatedWidgets();
	if (widgets.isEmpty())
		return;

	if (Configuration.emoticonTheme().emoticons().isEmpty())
		return;

	EmoticonPathProvider *emoticonPathProvider = Configuration.animate()
			? static_cast<EmoticonPathProvider *>(new AnimatedEmoticonPathProvider())
			: static_cast<EmoticonPathProvider *>(new StaticEmoticonPathProvider());

	EmoticonSelector *emoticonSelector = new EmoticonSelector(Configuration.emoticonTheme().emoticons(), emoticonPathProvider,
			widgets.at(widgets.size() - 1));
	connect(emoticonSelector, SIGNAL(emoticonClicked(QString)), chatEditBox, SLOT(insertPlainText(QString)));
	emoticonSelector->show();
}

void InsertEmoticonAction::setConfiguration(const EmoticonConfiguration &configuration)
{
	Configuration = configuration;

	foreach (Action *action, actions())
		updateActionState(action);
}

#include "moc_insert-emoticon-action.cpp"
