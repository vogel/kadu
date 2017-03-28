/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QPointer>
#include <QtGui/QSyntaxHighlighter>

class SpellChecker;

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

    static QList<Highlighter *> Highlighters;
    static QTextCharFormat HighlightFormat;

public:
    explicit Highlighter(SpellChecker *spellChecker, QTextDocument *document);
    virtual ~Highlighter();

    virtual void highlightBlock(const QString &text);

    static void rehighlightAll();
    static void setHighlightFormat(const QTextCharFormat &format);
    static void removeAll();

private:
    QPointer<SpellChecker> m_spellChecker;
};
