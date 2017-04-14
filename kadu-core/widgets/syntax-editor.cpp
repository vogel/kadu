/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>

#include "core/injected-factory.h"
#include "misc/paths-provider.h"
#include "misc/syntax-list.h"
#include "windows/message-dialog.h"

#include "syntax-editor.h"
#include "syntax-editor.moc"

SyntaxEditor::SyntaxEditor(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    syntaxListCombo = new QComboBox(this);
    connect(syntaxListCombo, SIGNAL(activated(const QString &)), this, SLOT(syntaxChangedSlot(const QString &)));

    layout->addWidget(syntaxListCombo, 100);
}

SyntaxEditor::~SyntaxEditor()
{
}

void SyntaxEditor::setInjectedFactory(InjectedFactory *injectedFactory)
{
    m_injectedFactory = injectedFactory;
}

void SyntaxEditor::setPathsProvider(PathsProvider *pathsProvider)
{
    m_pathsProvider = pathsProvider;
}

void SyntaxEditor::setCurrentSyntax(const QString &syntax)
{
    syntaxListCombo->setCurrentIndex(syntaxListCombo->findText(syntax));
    syntaxChangedSlot(syntax);
}

QString SyntaxEditor::currentSyntax()
{
    return syntaxListCombo->currentText();
}

void SyntaxEditor::setCategory(const QString &category)
{
    if (this->category == category)
        return;

    this->category = category;
    updateSyntaxList();
}

void SyntaxEditor::setSyntaxHint(const QString &syntaxHint)
{
    this->syntaxHint = syntaxHint;
}

void SyntaxEditor::syntaxChangedSlot(const QString &newSyntax)
{
    if (!syntaxList)
        return;

    if (!syntaxList->contains(newSyntax))
        return;

    QFile file;
    QString fileName;
    QString content;

    SyntaxInfo info = (*syntaxList)[newSyntax];
    if (info.global)
        fileName = m_pathsProvider->dataPath() + QStringLiteral("syntax/") + category.toLower() + '/' + newSyntax +
                   QStringLiteral(".syntax");
    else
        fileName = m_pathsProvider->profilePath() + QStringLiteral("syntax/") + category.toLower() + '/' + newSyntax +
                   QStringLiteral(".syntax");

    file.setFileName(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    content = stream.readAll();
    file.close();

    emit syntaxChanged(content);
}

void SyntaxEditor::updateSyntaxList()
{
    syntaxList = QSharedPointer<SyntaxList>(m_injectedFactory->makeInjected<SyntaxList>(category.toLower()));

    syntaxListCombo->clear();
    syntaxListCombo->addItems(syntaxList->keys());

    connect(syntaxList.data(), SIGNAL(updated()), this, SLOT(syntaxListUpdated()));
}

void SyntaxEditor::syntaxListUpdated()
{
    syntaxListCombo->clear();
    syntaxListCombo->addItems(syntaxList->keys());
}
