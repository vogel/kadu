/*
 * %kadu copyright begin%
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QStringList>
#include <QtGui/QTextCursor>
#include <QtWidgets/QAction>
#include <injeqt/injeqt.h>

class ActionDescription;
class Actions;
class CustomInputMenuManager;
class PluginInjectedFactory;
class SpellcheckerConfiguration;
class SpellChecker;

class Suggester : public QObject
{
    Q_OBJECT

    QPointer<Actions> m_actions;
    QPointer<CustomInputMenuManager> m_customInputMenuManager;
    QPointer<PluginInjectedFactory> m_pluginInjectedFactory;
    QPointer<SpellcheckerConfiguration> m_spellcheckerConfiguration;
    QPointer<SpellChecker> m_spellChecker;

    QStringList SuggestionWordList;
    QTextCursor CurrentTextSelection;
    QList<ActionDescription *> SuggestActions;

    void buildSuggestList(const QString &word);
    void addWordListToMenu(const QTextCursor &textCursor);

private slots:
    INJEQT_SET void setActions(Actions *actions);
    INJEQT_SET void setCustomInputMenuManager(CustomInputMenuManager *customInputMenuManager);
    INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);
    INJEQT_SET void setSpellcheckerConfiguration(SpellcheckerConfiguration *spellcheckerConfiguration);
    INJEQT_SET void setSpellChecker(SpellChecker *spellChecker);

protected:
    virtual bool eventFilter(QObject *object, QEvent *event);

public:
    Q_INVOKABLE explicit Suggester(QObject *parent = nullptr);
    virtual ~Suggester();

    void clearWordMenu();

public slots:
    void replaceWith(QString word);
};
