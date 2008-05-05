#ifndef EMOTICONS_H
#define EMOTICONS_H

#include <QList>
#include <QPair>
#include <QToolButton>

#include "configuration_aware_object.h"
#include "html_document.h"
#include "themes.h"

class ChatWidget;
class EmotsWalker;

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
	struct EmoticonsListItem
	{
		QString alias;
		QString anim;
		QString stat;
		EmoticonsListItem();
	};
	QList<EmoticonsListItem> Aliases;
	QList<EmoticonsListItem> Selector;
	EmotsWalker *walker;

	static QString getQuoted(const QString &s, unsigned int &pos);

	bool loadGGEmoticonThemePart(const QString &subdir);
	bool loadGGEmoticonTheme();

public:
	static void initModule();
	static void closeModule();

	/**
		Konstruktor tworzy obiekt oraz wyszukuje listê zestawów w katalogu
		$DATADIR/kadu/themes/emoticons
	**/
	EmoticonsManager(const QString &name, const QString &configname);
	~EmoticonsManager();

	/**
		\brief Funkcja ustawia zestaw emotikonek na theme
		\param theme nazwa zestawu
		Funkcja ustawi zestaw je¶li taki istnieje w przeciwnym wypadku
		ustawionym zestawem bêdzie "gadu-gadu"
	**/
	void setEmoticonsTheme(const QString &theme);

	/**
		Funkcja wstawia w text zamiast tagów emotikonek odpowiednie emotikonki
		\param text dokument w którym maj± byæ zamienione tagi
		\param bgcolor kolor t³a emotikonki
		\param style styl emotikonki jaki ma byæ wstawiony - domy¶lnie jest do
		 aktualny zestaw
	**/
	void expandEmoticons(HtmlDocument &text, const QColor &bgcolor, EmoticonsStyle style);

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

	QString EmoticonString;
	QString AnimPath;
	QString StaticPath;
	QMovie *Movie;

private slots:
	void buttonClicked();
	void movieUpdate();

protected:
	/**
		Funkcja obs³uguj±ca najechanie kursorem myszki na dan± emotikonkê.
	**/
	void enterEvent(QEvent *e);

	/**
		Funkcja obs³uguj±ca opuszczenie obszaru wy¶wietlania emotikonki.
	**/
	void leaveEvent(QEvent *e);

public:
	/**
		Konstruktor tworz±cy przycisk z emotikonk±
		\param parent widget na którym osadzona ma byæ przycisk z emotikonk±
		\param emoticon_string nazwa która ma byæ wy¶wietlana po najechaniu na
		przycisk
		\param static_path ¶cie¿ka do statycznej emotikonki
		\param anim_path ¶cie¿ka do animowanej emotikonki
	**/
	EmoticonSelectorButton(const QString &emoticon_string, const QString &static_path, const QString &anim_path, QWidget *parent);
	~EmoticonSelectorButton();
	
signals:
	/**
		Sygna³ emitowany po naci¶niêciu przycisku z emotikonk±
		przekazywana jest warto¶æ emoticon_string (przewa¿nie jest to tag)
	**/
	void clicked(const QString &emoticon_string);

};
/**
	Klasa wy¶wietlaj±ca listê emotikonek z aktualnego zestawu.
**/
class EmoticonSelector : public QWidget
{
	Q_OBJECT

	ChatWidget *callingwidget;

private slots:
	void iconClicked(const QString &emoticon_string);

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
	EmoticonSelector(ChatWidget *caller, QWidget *parent = 0);

public slots:
	/**
		Slot obs³uguj±cy poprawne wy¶wietlenie listy emotikonek, wyrównanie do
		okna wywo³uj±cego.
	**/
	void alignTo(QWidget *w);

};

struct PrefixNode
{
	int emotIndex;
	QList<QPair<QChar, PrefixNode *> > childs;
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
	PrefixNode *root;
	QPair<QChar, PrefixNode *> myPair;
	/** positions in prefix tree, representing current analysis of text */
	QList<const PrefixNode *> positions;
	QList<int> lengths;
	int amountPositions;

	PrefixNode * findChild(const PrefixNode *node, const QChar &c);
	PrefixNode * insertChild(PrefixNode *node, const QChar &c);
	void removeChilds(PrefixNode *node);

public:
	EmotsWalker();
	~EmotsWalker();

	/**
		adds given string (emot) to dictionary of emots, giving it
		number, which will be used later to notify occurrences of
		emot in analyzed text
	*/
	void insertString(const QString &str, int num);

	/**
		return number of emot, which occurre in analyzed text just
		after adding given character (thus ending on this character)
		beginning of text analysis is turned on by 'initWalking()'
		if no emot occures, -1 is returned
	*/
	int checkEmotOccurrence(const QChar &c);

	/**
		clear internal structures responsible for analyzing text, it allows
		begin of new text analysis
	*/
	void initWalking();

};

#endif
