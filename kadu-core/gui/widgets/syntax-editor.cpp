/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtGui/QComboBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>

#include "gui/windows/message-dialog.h"
#include "misc/kadu-paths.h"
#include "misc/syntax-list.h"

#include "syntax-editor.h"

SyntaxEditor::SyntaxEditor(QWidget *parent) :
		QWidget(parent)
{
	QHBoxLayout *layout = new QHBoxLayout(this);

	syntaxListCombo = new QComboBox(this);
	connect(syntaxListCombo, SIGNAL(activated(const QString &)), this, SLOT(syntaxChangedSlot(const QString &)));

	layout->addWidget(syntaxListCombo, 100);
}

SyntaxEditor::~SyntaxEditor()
{
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
		fileName = KaduPaths::instance()->dataPath() + QLatin1String("syntax/") + category.toLower() + '/' + newSyntax + QLatin1String(".syntax");
	else
		fileName = KaduPaths::instance()->profilePath() + QLatin1String("syntax/") + category.toLower() + '/' + newSyntax + QLatin1String(".syntax");

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
	syntaxList = QSharedPointer<SyntaxList>(new SyntaxList(category.toLower()));

	syntaxListCombo->clear();
	syntaxListCombo->addItems(syntaxList->keys());

	connect(syntaxList.data(), SIGNAL(updated()), this, SLOT(syntaxListUpdated()));
}

void SyntaxEditor::syntaxListUpdated()
{
	syntaxListCombo->clear();
	syntaxListCombo->addItems(syntaxList->keys());
}

#include "moc_syntax-editor.cpp"
