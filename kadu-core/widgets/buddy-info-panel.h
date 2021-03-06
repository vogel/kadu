/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "kadu-web-view.h"

#include "configuration/configuration-aware-object.h"
#include "contacts/contact.h"
#include "talkable/talkable.h"

#include <injeqt/injeqt.h>

class Avatars;
class BuddyPreferredManager;
class DomProcessorService;
class Parser;
class PathsProvider;
class TalkableConverter;
struct AvatarId;

class BuddyInfoPanel : public KaduWebView, private ConfigurationAwareObject
{
    Q_OBJECT

    QPointer<Avatars> m_avatars;
    QPointer<BuddyPreferredManager> m_buddyPreferredManager;
    QPointer<DomProcessorService> m_domProcessorService;
    QPointer<Parser> m_parser;
    QPointer<PathsProvider> m_pathsProvider;
    QPointer<TalkableConverter> m_talkableConverter;

    Talkable Item;
    QString Template;
    QString Syntax;
    QString BackgroundColor;

    void avatarUpdated(const AvatarId &id);

    void connectItem();
    void disconnectItem();

private slots:
    INJEQT_SET void setAvatars(Avatars *avatars);
    INJEQT_SET void setBuddyPreferredManager(BuddyPreferredManager *buddyPreferredManager);
    INJEQT_SET void setDomProcessorService(DomProcessorService *domProcessorService);
    INJEQT_SET void setParser(Parser *parser);
    INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);
    INJEQT_SET void setTalkableConverter(TalkableConverter *talkableConverter);
    INJEQT_INIT void init();

    void buddyUpdated(const Buddy &buddy);

protected:
    virtual void configurationUpdated();

public:
    explicit BuddyInfoPanel(QWidget *parent = nullptr);
    virtual ~BuddyInfoPanel();

    virtual void setVisible(bool visible);

public slots:
    void displayItem(Talkable item);
    void update();
    void styleFixup(QString &syntax);
};
