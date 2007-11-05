#ifndef EMOTICONS_H
#define EMOTICONS_H

#include <qcolor.h>
#include <qmovie.h>
#include <private/qrichtext_p.h>
#include <qstringlist.h>
#include <qtoolbutton.h>
#include <qvaluevector.h>
#include <qlabel.h>

#include "config_file.h"
#include "configuration_aware_object.h"
#include "html_document.h"
#include "themes.h"

class ChatWidget;
class EmotsWalker;
class QImage;
class QLabel;
class QTextEdit;
class QScrollView;

enum EmoticonsStyle
{
	EMOTS_NONE,
	EMOTS_STATIC,
	EMOTS_ANIMATED
};

/**
	Menad¿er emotikonów
**/
class EmoticonsManager : public Themes, ConfigurationAwareObject
{
	private:
		struct EmoticonsListItem
		{
			QString alias;
			QString anim;
			QString stat;
			EmoticonsListItem();
		};
		QValueVector<EmoticonsListItem> Aliases;
		QValueList<EmoticonsListItem> Selector;
		EmotsWalker *walker;

		static QString getQuoted(const QString& s, unsigned int& pos);

		bool loadGGEmoticonThemePart(QString subdir);
		bool loadGGEmoticonTheme();
	public:
		static void initModule();
		static void closeModule();

		/**
		Konstruktor tworzy obiekt oraz wyszukuje listê zestawów w katalogu
		$DATADIR/kadu/themes/emoticons
		**/
		EmoticonsManager(const QString& name, const QString& configname);
		~EmoticonsManager();

		/**
		\brief Funkcja ustawia zestaw emotikonek na theme
		\param theme nazwa zestawu
		Funkcja ustawi zestaw je¶li taki istnieje w przeciwnym wypadku
		ustawionym zestawem bêdzie "gadu-gadu"
		**/
		void setEmoticonsTheme(const QString& theme);

		/**
		Funkcja wstawia w text zamiast tagów emotikonek odpowiednie emotikonki
		\param text dokument w którym maj± byæ zamienione tagi
		\param bgcolor kolor t³a emotikonki
		\param style styl emotikonki jaki ma byæ wstawiony - domy¶lnie jest do
		 aktualny zestaw
		**/
		void expandEmoticons(HtmlDocument& text, const QColor& bgcolor,
			EmoticonsStyle style=(EmoticonsStyle) config_file.readNumEntry("Chat", "EmoticonsStyle"));
		/**
		Funkcja zwraca ilo¶æ emotikonek w zestawie
		**/
		int selectorCount() const;
		/**
		Funkcja zwraca alias odpowiedniej emotikonki
		\param emot_num nr emotikonki z listy
		\return Zwracany jest pusty ci±g je¶li nie znaleziono emotikonki w przeciwnym razie zwracany jest alias.
		**/
		QString selectorString(int emot_num) const;
		/**
		Funkcja zwraca ¶cie¿kê do odpowiedniej animowanej emotikonki
		\param emot_num nr emotikonki z listy
		\return Zwracany jest pusty ci±g je¶li nie znaleziono emotikonki w przeciwnym razie zwracana jest ¶cie¿ka do emotikonki
		**/
		QString selectorAnimPath(int emot_num) const;
		/**
		Funkcja zwraca ¶cie¿kê do odpowiedniej statycznej emotikonki
		\param emot_num nr emotikonki z listy
		\return Zwracany jest pusty ci±g je¶li nie znaleziono emotikonki w przeciwnym razie zwracana jest ¶cie¿ka do emotikonki
		**/
		QString selectorStaticPath(int emot_num) const;

		virtual void configurationUpdated();
};

extern EmoticonsManager *emoticons;

/**
	Klasa s³u¿±ca do wyboru emotikonki z zestawu
**/
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
		/**
		Funkcja obs³uguj±ca najechanie kursorem myszki na dan± emotikonkê.
		**/
		void enterEvent(QEvent* e);
		/**
		Funkcja obs³uguj±ca opuszczenie obszaru wy¶wietlania emotikonki.
		**/
		void leaveEvent(QEvent* e);
	public:
		/**
		Konstruktor tworz±cy przycisk z emotikonk±
		\param parent widget na którym osadzona ma byæ przycisk z emotikonk±
		\param emoticon_string nazwa która ma byæ wy¶wietlana po najechaniu na
		przycisk
		\param static_path ¶cie¿ka do statycznej emotikonki
		\param anim_path ¶cie¿ka do animowanej emotikonki
		**/
		EmoticonSelectorButton(
			QWidget* parent,const QString& emoticon_string,
			const QString& static_path,const QString& anim_path);
		~EmoticonSelectorButton();
	signals:
		/**
		Sygna³ emitowany po naci¶niêciu przycisku z emotikonk±
		przekazywana jest warto¶æ emoticon_string (przewa¿nie jest to tag)
		**/
		void clicked(const QString& emoticon_string);
};
/**
	Klasa wy¶wietlaj±ca listê emotikonek z aktualnego zestawu.
**/
class EmoticonSelector : public QWidget
{
	Q_OBJECT
	private:
		ChatWidget *callingwidget;
	private slots:
		void iconClicked(const QString& emoticon_string);
	protected:
		/**
		Funkcja obs³uguj±ca zamkniêcie listy.
		**/
		void closeEvent(QCloseEvent *e);
	public:
		/**
		Konstruktor tworz±cy listê emotikonek.
		\param parent rodzic na którym ma byæ wy¶wietlona lista
		\param name nazwa obiektu
		\param caller okno chat do ktorego ma byæ wpisana wybrana emotikonka
		**/
		EmoticonSelector(QWidget* parent = 0, const char *name = 0, ChatWidget *caller = 0);
	public slots:
		/**
		Slot obs³uguj±cy poprawne wy¶wietlenie listy emotikonek, wyrównanie do
		okna wywo³uj±cego.
		**/
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

class AnimatedLabel;

class AnimTextItem : public QTextCustomItem
{
	public:
		struct MovieCacheData
		{
			QMovie movie;
			QSize size;
			int count;
			int runCount;
			MovieCacheData(const QString &fileName);
		};
	private:
		static QImage* SizeCheckImage;
		typedef QMap<QString,MovieCacheData*> MoviesCache;
		static MoviesCache* Movies;
		QTextEdit* Edit;
		AnimatedLabel* Label;
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

class AnimatedLabel : public QLabel
{
	Q_OBJECT
	public:
	AnimTextItem::MovieCacheData *movieData;
	QScrollView *scrollView;
	QString tip;
	static bool mustPause;
	bool imageBackground;
	bool paused;
	int lastX, lastY;
	bool trueTransparency;
	public:
		AnimatedLabel(AnimTextItem::MovieCacheData *data, const QString &tip, bool imageBackground,
						QScrollView *view, bool trueTransparency = false, const char *name = 0);
		~AnimatedLabel();
	public slots:
		void unpauseMovie();
		void pauseMovie();
	protected:
		virtual void paintEvent(QPaintEvent *e);
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
	PrefixNode();
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
	QValueVector<const PrefixNode*> positions;
	QValueVector<int> lengths;
	unsigned int amountPositions;

	public:
		EmotsWalker();
		~EmotsWalker();

	private:
		PrefixNode* findChild( const PrefixNode* node, const QChar& c );
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
