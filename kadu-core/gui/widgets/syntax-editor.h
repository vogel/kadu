/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SYNTAX_EDITOR_H
#define SYNTAX_EDITOR_H

#include <QtGui/QWidget>

class QComboBox;
class QPushButton;

class SyntaxEditorWindow;
class SyntaxList;

class SyntaxEditor : public QWidget
{
	Q_OBJECT

	SyntaxList *syntaxList;
	QComboBox *syntaxListCombo;
	QPushButton *deleteButton;

	QString category;
	QString syntaxHint;

	void updateSyntaxList();

private slots:
	void editClicked();
	void deleteClicked();

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
	void onSyntaxEditorWindowCreated(SyntaxEditorWindow *syntaxEditorWindow);

};

#endif // SYNTAX_EDITOR_H
