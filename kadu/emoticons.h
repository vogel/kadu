/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef EMOTICONS_H
#define EMOTICONS_H

#include <qvaluelist.h>
#include <qstringlist.h>
#include <qregexp.h>

class EmoticonsManager
{
	private:
		QStringList ThemesList;
		struct EmoticonsRegexpListItem
		{
			QRegExp regexp;
			QString picname;
		};
		QValueList<EmoticonsRegexpListItem> EmoticonsRegexpList;
		struct EmoticonsSelectorListItem
		{
			QString string;
			QString picname;
		};
		QValueList<EmoticonsSelectorListItem> EmoticonsSelectorList;
		void loadEmoticonsRegexpList();
		void loadEmoticonsSelectorList();
	public:
		EmoticonsManager();
		const QStringList& themes();
		void setEmoticonsTheme(const QString& theme);
		QString themePath();
		void expandEmoticons(QString& text,const QColor& bgcolor);
		int emoticonsCount();
		QString emoticonString(int emot_num);
		QString emoticonPicPath(int emot_num);
};

extern EmoticonsManager emoticons;

#include <qtextedit.h>
#include <private/qrichtext_p.h>
#include <qmovie.h>
#include <qlabel.h>

class AnimTextItem : public QTextCustomItem
{
	private:
		QTextEdit* Editor;
		QLabel* Label;
	public:
		AnimTextItem(
			QTextDocument *p, QTextEdit* edit,
			const QString& filename, const QColor& bgcolor );
		~AnimTextItem();
		void draw(
			QPainter* p, int x, int y, int cx, int cy,
			int cw, int ch, const QColorGroup& cg,
			bool selected );
};

class AnimStyleSheet : public QStyleSheet
{
private:
	QTextEdit* Editor;
	QString Path;
public:
	AnimStyleSheet(
		QTextEdit* parent, const QString& path, const char* name = 0 );
	QTextCustomItem* tag(
		const QString& name, const QMap<QString,QString>& attr,
		const QString& context, const QMimeSourceFactory& factory,
		bool emptyTag, QTextDocument* doc) const;
};

#endif
