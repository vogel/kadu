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
#include <qtextedit.h>
#include <private/qrichtext_p.h>
#include <qlabel.h>
#include <qimage.h>
#include <qtoolbutton.h>
#include <qmovie.h>

#include "chat.h"

class EmoticonsManager
{
	private:
		QStringList ThemesList;
		struct EmoticonsListItem
		{
			QString alias;
			QString anim;
			QString stat;
		};
		QValueList<EmoticonsListItem> Aliases;
		QValueList<EmoticonsListItem> Selector;
		QStringList getSubDirs(const QString& path);
		QString getQuoted(const QString& s,int& pos);
		QString fixFileName(const QString& path,const QString& fn);
		bool loadGGEmoticonThemePart(QString subdir);
		bool loadGGEmoticonTheme();
	public:
		EmoticonsManager();
		const QStringList& themes();
		void setEmoticonsTheme(const QString& theme);
		QString themePath();
		void expandEmoticons(QString& text,const QColor& bgcolor);
		int selectorCount();
		QString selectorString(int emot_num);
		QString selectorAnimPath(int emot_num);
		QString selectorStaticPath(int emot_num);
};

extern EmoticonsManager emoticons;

class EmoticonSelectorButton : public QToolButton
{
	Q_OBJECT	
	private:
		QString EmoticonString;
		QString AnimPath;
		QString StaticPath;
		QMovie* Movie;
	private slots:
		void buttonClicked();
		void movieUpdate();
	protected:
		void enterEvent(QEvent* e);
		void leaveEvent(QEvent* e);
	public:
		EmoticonSelectorButton(
			QWidget* parent,const QString& emoticon_string,
			const QString& static_path,const QString& anim_path);	
	signals:
		void clicked(const QString& emoticon_string);
};

class EmoticonSelector : public QWidget
{
	Q_OBJECT
	private:
		Chat *callingwidget;
	private slots:
		void iconClicked(const QString& emoticon_string);
	protected:
		void closeEvent(QCloseEvent *e);
	public:
		EmoticonSelector(QWidget* parent = 0, const char *name = 0, Chat *caller = 0);
		void alignTo(QWidget* w);
};

class AnimTextItem : public QTextCustomItem
{
	private:
		static QImage* SizeCheckImage;
		QTextEdit* Edit;
		QLabel* Label;
		QSize EditSize;
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
