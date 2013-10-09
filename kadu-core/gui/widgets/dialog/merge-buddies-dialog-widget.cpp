/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QAction>
#include <QtGui/QFormLayout>
#include <QtGui/QIcon>
#include <QtGui/QLabel>

#include "buddies/buddy-manager.h"
#include "buddies/model/buddy-list-model.h"
#include "buddies/model/buddy-manager-adapter.h"
#include "core/core.h"
#include "gui/widgets/select-talkable-combo-box.h"
#include "icons/kadu-icon.h"
#include "talkable/filter/exclude-buddy-talkable-filter.h"


#include "merge-buddies-dialog-widget.h"

MergeBuddiesDialogWidget::MergeBuddiesDialogWidget(Buddy buddy, const QString &message, QWidget *parent) :
		DialogWidget(tr("Merge Buddies"), message, QPixmap(), parent), MyBuddy(buddy)
{
	QIcon icon = KaduIcon("kadu_icons/kadu").icon();
	Pixmap = icon.pixmap(icon.actualSize(QSize(64, 64)));

	createGui();
}

MergeBuddiesDialogWidget::~MergeBuddiesDialogWidget()
{
}

void MergeBuddiesDialogWidget::createGui()
{
	QFormLayout *formLayout = new QFormLayout(this);
	formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

	QLabel *selectLabel = new QLabel(tr("Buddy to merge") + ":", this);

	SelectCombo = new SelectTalkableComboBox(this);
	SelectCombo->addBeforeAction(new QAction(tr(" - Select buddy - "), SelectCombo));

	BuddyListModel *buddyListModel = new BuddyListModel(SelectCombo);
	new BuddyManagerAdapter(buddyListModel);
	SelectCombo->setBaseModel(buddyListModel);
	SelectCombo->addFilter(new ExcludeBuddyTalkableFilter(MyBuddy, SelectCombo));
	SelectCombo->addFilter(new ExcludeBuddyTalkableFilter(Core::instance()->myself(), SelectCombo));
	connect(SelectCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedBuddyChanged()));
	formLayout->addRow(selectLabel, SelectCombo);

	setLayout(formLayout);
}

void MergeBuddiesDialogWidget::selectedBuddyChanged()
{
	emit valid(SelectCombo->currentTalkable().isValidBuddy());
}

void MergeBuddiesDialogWidget::dialogAccepted()
{
	Buddy mergeWith = SelectCombo->currentTalkable().toBuddy();

	if (mergeWith.isNull() || MyBuddy.isNull())
		return;

	BuddyManager::instance()->mergeBuddies(mergeWith, MyBuddy);
}

void MergeBuddiesDialogWidget::dialogRejected()
{
}

#include "moc_merge-buddies-dialog-widget.cpp"
