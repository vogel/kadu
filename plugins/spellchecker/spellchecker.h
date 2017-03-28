/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2011, 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/gui/configuration-ui-handler.h"

#include <QtCore/QMap>
#include <QtCore/QPointer>
#include <QtCore/QString>
#include <injeqt/injeqt.h>

class QListWidget;
class QListWidgetItem;

class ChatWidget;
class ChatWidgetRepository;
class IconsManager;
class PathsProvider;
class SpellcheckerConfiguration;
class Suggester;

#if defined(HAVE_ASPELL)
struct AspellSpeller;
struct AspellConfig;
#elif defined(HAVE_ENCHANT)
typedef struct str_enchant_broker EnchantBroker;
typedef struct str_enchant_dict EnchantDict;
#endif

class SpellChecker : public QObject
{
    Q_OBJECT

public:
#if defined(HAVE_ASPELL)
    typedef QMap<QString, AspellSpeller *> Checkers;
#elif defined(HAVE_ENCHANT)
    typedef QMap<QString, EnchantDict *> Checkers;
#endif

    Q_INVOKABLE explicit SpellChecker(QObject *parent = nullptr);
    virtual ~SpellChecker();

    void buildMarkTag();
    void buildCheckers();
    bool checkWord(const QString &word);
    QStringList buildSuggestList(const QString &word);

    QStringList notCheckedLanguages() const;
    QStringList checkedLanguages() const;
    bool addCheckedLang(const QString &name);
    void removeCheckedLang(const QString &name);

public slots:
    void chatWidgetAdded(ChatWidget *chatWidget);

private:
    QPointer<ChatWidgetRepository> m_chatWidgetRepository;
    QPointer<IconsManager> m_iconsManager;
    QPointer<PathsProvider> m_pathsProvider;
    QPointer<SpellcheckerConfiguration> m_spellcheckerConfiguration;
    QPointer<Suggester> m_suggester;

#if defined(HAVE_ASPELL)
    AspellConfig *SpellConfig;
#elif defined(HAVE_ENCHANT)
    EnchantBroker *Broker;
#endif

    Checkers MyCheckers;

private slots:
    INJEQT_SET void setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository);
    INJEQT_SET void setIconsManager(IconsManager *iconsManager);
    INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);
    INJEQT_SET void setSpellcheckerConfiguration(SpellcheckerConfiguration *spellcheckerConfiguration);
    INJEQT_SET void setSuggester(Suggester *suggester);
    INJEQT_INIT void init();

    void configurationUpdated();
};
