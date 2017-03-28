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

#pragma once

#include "buddies/buddy.h"
#include "widgets/dialog/dialog-widget.h"

#include <QtCore/QPointer>
#include <QtCore/QVariant>
#include <QtWidgets/QDialog>

class BuddyManager;
class IconsManager;
class InjectedFactory;
class Myself;
class SelectTalkableComboBox;
class TalkableConverter;

class MergeBuddiesDialogWidget : public DialogWidget
{
    Q_OBJECT

public:
    explicit MergeBuddiesDialogWidget(Buddy buddy, const QString &message, QWidget *parent);
    virtual ~MergeBuddiesDialogWidget();

private:
    QPointer<BuddyManager> m_buddyManager;
    QPointer<IconsManager> m_iconsManager;
    QPointer<InjectedFactory> m_injectedFactory;
    QPointer<Myself> m_myself;
    QPointer<TalkableConverter> m_talkableConverter;

    Buddy MyBuddy;

    SelectTalkableComboBox *SelectCombo;

    virtual void createGui();

private slots:
    INJEQT_SET void setBuddyManager(BuddyManager *buddyManager);
    INJEQT_SET void setIconsManager(IconsManager *iconsManager);
    INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
    INJEQT_SET void setMyself(Myself *myself);
    INJEQT_SET void setTalkableConverter(TalkableConverter *talkableConverter);
    INJEQT_INIT void init();

    void selectedBuddyChanged();
    virtual void dialogAccepted();
    virtual void dialogRejected();
};
