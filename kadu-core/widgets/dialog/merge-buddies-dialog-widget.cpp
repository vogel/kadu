/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "merge-buddies-dialog-widget.h"
#include "merge-buddies-dialog-widget.moc"

#include "buddies/buddy-manager.h"
#include "buddies/model/buddy-list-model.h"
#include "buddies/model/buddy-manager-adapter.h"
#include "core/injected-factory.h"
#include "core/myself.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "talkable/filter/exclude-buddy-talkable-filter.h"
#include "talkable/talkable-converter.h"
#include "widgets/select-talkable-combo-box.h"

#include <QtGui/QIcon>
#include <QtWidgets/QAction>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>

MergeBuddiesDialogWidget::MergeBuddiesDialogWidget(Buddy buddy, const QString &message, QWidget *parent)
        : DialogWidget(tr("Merge Buddies"), message, QPixmap(), parent), MyBuddy(buddy)
{
}

MergeBuddiesDialogWidget::~MergeBuddiesDialogWidget()
{
}

void MergeBuddiesDialogWidget::setBuddyManager(BuddyManager *buddyManager)
{
    m_buddyManager = buddyManager;
}

void MergeBuddiesDialogWidget::setIconsManager(IconsManager *iconsManager)
{
    m_iconsManager = iconsManager;
}

void MergeBuddiesDialogWidget::setInjectedFactory(InjectedFactory *injectedFactory)
{
    m_injectedFactory = injectedFactory;
}

void MergeBuddiesDialogWidget::setMyself(Myself *myself)
{
    m_myself = myself;
}

void MergeBuddiesDialogWidget::setTalkableConverter(TalkableConverter *talkableConverter)
{
    m_talkableConverter = talkableConverter;
}

void MergeBuddiesDialogWidget::init()
{
    QIcon icon = m_iconsManager->iconByPath(KaduIcon("kadu_icons/kadu"));
    Pixmap = icon.pixmap(icon.actualSize(QSize(64, 64)));

    createGui();
}

void MergeBuddiesDialogWidget::createGui()
{
    QFormLayout *formLayout = new QFormLayout(this);
    formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    QLabel *selectLabel = new QLabel(tr("Buddy to merge") + ":", this);

    SelectCombo = m_injectedFactory->makeInjected<SelectTalkableComboBox>(this);
    SelectCombo->addBeforeAction(new QAction(tr(" - Select buddy - "), SelectCombo));

    auto buddyListModel = m_injectedFactory->makeInjected<BuddyListModel>(SelectCombo);
    m_injectedFactory->makeInjected<BuddyManagerAdapter>(buddyListModel);
    SelectCombo->setBaseModel(buddyListModel);
    SelectCombo->addFilter(new ExcludeBuddyTalkableFilter(MyBuddy, SelectCombo));
    SelectCombo->addFilter(new ExcludeBuddyTalkableFilter(m_myself->buddy(), SelectCombo));
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
    Buddy mergeWith = m_talkableConverter->toBuddy(SelectCombo->currentTalkable());

    if (mergeWith.isNull() || MyBuddy.isNull())
        return;

    m_buddyManager->mergeBuddies(mergeWith, MyBuddy);
}

void MergeBuddiesDialogWidget::dialogRejected()
{
}
