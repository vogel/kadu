/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#ifndef SYNTAX_EDITOR_H
#define SYNTAX_EDITOR_H

#include <QtCore/QSharedPointer>
#include <QtWidgets/QWidget>

class QComboBox;
class QPushButton;

class SyntaxList;

class SyntaxEditor : public QWidget
{
	Q_OBJECT

	QSharedPointer<SyntaxList> syntaxList;
	QComboBox *syntaxListCombo;

	QString category;
	QString syntaxHint;

	void updateSyntaxList();

private slots:
	void syntaxChangedSlot(const QString &newSyntax);
	void syntaxListUpdated();

public:
	SyntaxEditor(QWidget *parent = 0);
	virtual ~SyntaxEditor();

	QString currentSyntax();

	void setCategory(const QString &category);
	void setSyntaxHint(const QString &syntaxHint);

public slots:
	void setCurrentSyntax(const QString &syntax);

signals:
	void syntaxChanged(const QString &newSyntax);

};

#endif // SYNTAX_EDITOR_H
