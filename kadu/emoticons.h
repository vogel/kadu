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
		void expandEmoticons(QString& text);
		int emoticonsCount();
		QString emoticonString(int emot_num);
		QString emoticonPicPath(int emot_num);
};

extern EmoticonsManager emoticons;

#endif
