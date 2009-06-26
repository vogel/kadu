/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SYNTAX_EDITOR2_H
#define SYNTAX_EDITOR2_H

#include <QtCore/QMap>
#include <QtGui/QWidget>

class QComboBox;
class QLineEdit;
class QPushButton;
class QTextEdit;

class Preview;
class SyntaxEditorWindow;

struct SyntaxInfo
{
	bool global;
};

class SyntaxList : public QObject, public QMap<QString, SyntaxInfo>
{
	Q_OBJECT

	QString category;

public:
	SyntaxList(const QString &category);
	virtual ~SyntaxList() {}

	static QString readSyntax(const QString &category, const QString &name, const QString &defaultSyntax);

	void reload();

	bool updateSyntax(const QString &name, const QString &syntax);
	QString readSyntax(const QString &name);
	bool deleteSyntax(const QString &name);

	bool isGlobal(const QString &name);

signals:
	void updated();

};

#endif // SYNTAX_EDITOR2
