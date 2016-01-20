/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#pragma once

#include <QtCore/QPointer>
#include <QtCore/QSharedPointer>
#include <QtWidgets/QWidget>
#include <injeqt/injeqt.h>

class QComboBox;
class QPushButton;

class InjectedFactory;
class PathsProvider;
class SyntaxList;

class SyntaxEditor : public QWidget
{
	Q_OBJECT

public:
	explicit SyntaxEditor(QWidget *parent = nullptr);
	virtual ~SyntaxEditor();

	QString currentSyntax();

	void setCategory(const QString &category);
	void setSyntaxHint(const QString &syntaxHint);

public slots:
	void setCurrentSyntax(const QString &syntax);

signals:
	void syntaxChanged(const QString &newSyntax);

private:
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<PathsProvider> m_pathsProvider;

	QSharedPointer<SyntaxList> syntaxList;
	QComboBox *syntaxListCombo;

	QString category;
	QString syntaxHint;

	void updateSyntaxList();

private slots:
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);

	void syntaxChangedSlot(const QString &newSyntax);
	void syntaxListUpdated();

};
