#ifndef EMOTICONS_H
#define EMOTICONS_H

#include <qmovie.h>
#include <qmap.h>
#include <private/qrichtext_p.h>
#include <qstringlist.h>
#include <qtextedit.h>
#include <qtoolbutton.h>
#include <qvaluelist.h>
#include <qvaluevector.h>

#include "misc.h"

class Chat;
class EmotsWalker;
class QImage;
class QLabel;

enum EmoticonsStyle
{
	EMOTS_NONE,
	EMOTS_STATIC,
	EMOTS_ANIMATED
};

/**
	Menad¿er emotikonów
**/
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
		QValueVector<EmoticonsListItem> Aliases;
		QValueList<EmoticonsListItem> Selector;
		EmotsWalker *walker;

		static QStringList getSubDirs(const QString& path);
		static QString getQuoted(const QString& s, unsigned int& pos);
		static QString fixFileName(const QString& path,const QString& fn);
		bool loadGGEmoticonThemePart(QString subdir);
		bool loadGGEmoticonTheme();
	public:
		EmoticonsManager();
		~EmoticonsManager();
		const QStringList& themes() const;
		void setEmoticonsTheme(const QString& theme);
		QString themePath() const;
		void expandEmoticons(HtmlDocument& text, const QColor& bgcolor,
			EmoticonsStyle style=(EmoticonsStyle) config_file.readNumEntry("Chat", "EmoticonsStyle"));
		int selectorCount() const;
		QString selectorString(int emot_num) const;
		QString selectorAnimPath(int emot_num) const;
		QString selectorStaticPath(int emot_num) const;
};

extern EmoticonsManager *emoticons;

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
		~EmoticonSelectorButton();
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
	public slots:
		void alignTo(QWidget* w);
};

/* Klasa wyciêta za ¼róde³ Qt i przystosowana na potrzeby Kadu (oryginalnie nazywa³a siê QTextImage) */
class StaticTextItem : public QTextCustomItem
{
	public:
		StaticTextItem(QTextDocument *p, const QMap<QString, QString> &attr, const QString& context,
			QMimeSourceFactory &factory );
		virtual ~StaticTextItem();

		Placement placement() const { return place; }
		void adjustToPainter( QPainter* );
		int minimumWidth() const { return width; }

		QString richText() const;
		void draw(QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected);
	private:
		QRegion* reg;
		QPixmap pm;
		Placement place;
		int tmpwidth, tmpheight;
		QMap<QString, QString> attributes;
		QString imgId;
};

class AnimTextItem : public QTextCustomItem
{
	private:
		static QImage* SizeCheckImage;
		struct MovieCacheData
		{
			QMovie movie;
			QSize size;
			int count;
		};
		typedef QMap<QString,MovieCacheData> MoviesCache;
		static MoviesCache* Movies;
		QTextEdit* Edit;
		QLabel* Label;
		QSize EditSize;
		QString text;
		QString FileName;
	public:
		AnimTextItem(
			QTextDocument *p, QTextEdit* edit,
			const QString& filename, const QColor& bgcolor, const QString &tip);
		~AnimTextItem();
		void draw(
			QPainter* p, int x, int y, int cx, int cy,
			int cw, int ch, const QColorGroup& cg,
			bool selected );
		virtual QString richText() const;
};

class AnimStyleSheet : public QStyleSheet
{
	private:
		QString Path;
	public:
		AnimStyleSheet(QTextEdit* parent, const QString& path, const char* name = 0);
		QTextCustomItem* tag(
			const QString& name, const QMap<QString,QString>& attr,
			const QString& context, const QMimeSourceFactory& factory,
			bool emptyTag, QTextDocument* doc) const;
};

class StaticStyleSheet : public QStyleSheet
{
	private:
		QString Path;
	public:
		StaticStyleSheet(QTextEdit* parent, const QString& path, const char* name = 0);
		QTextCustomItem* tag(
			const QString& name, const QMap<QString,QString>& attr,
			const QString& context, const QMimeSourceFactory& factory,
			bool emptyTag, QTextDocument* doc) const;
};



struct PrefixNode 
{
  int emotIndex;
  QValueVector<QPair<QChar, PrefixNode*> > childs;
};

/**
	this class serves as dictionary of emots, allowing easy
	finding of their occurrences in text;
	new search is initialized by calling 'initWalking()'
	then characters are put into analysis by 'checkEmotOccurrence(c)'
*/
class EmotsWalker 
{
	/** dictionary is based on prefix tree */
	PrefixNode* root;
	QPair<QChar, PrefixNode*> myPair;
	/** positions in prefix tree, representing current analysis of text */
	QValueVector<PrefixNode*> positions;
	QValueVector<int> lengths;
	int amountPositions;

	public:
		EmotsWalker();
		~EmotsWalker();

	private:
		PrefixNode* findChild( PrefixNode* node, const QChar& c );
		PrefixNode* insertChild( PrefixNode* node, const QChar& c );
		void removeChilds( PrefixNode* node );

	public:
		/**
			adds given string (emot) to dictionary of emots, giving it
			number, which will be used later to notify occurrences of
			emot in analyzed text
		*/
		void insertString( const QString& str, int num );
		
		/**
			return number of emot, which occurre in analyzed text just
			after adding given character (thus ending on this character)
			beginning of text analysis is turned on by 'initWalking()'
			if no emot occures, -1 is returned
		*/
		int checkEmotOccurrence( const QChar& c );
		
		/**
			clear internal structures responsible for analyzing text, it allows 
			begin of new text analysis
		*/
		void initWalking();
};

#endif
