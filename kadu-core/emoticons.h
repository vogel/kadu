#ifndef EMOTICONS_H
#define EMOTICONS_H

#include <QtCore/QList>
#include <QtCore/QPair>
#include <QtGui/QLabel>
#include <QtGui/QScrollArea>

#include "configuration/configuration-aware-object.h"
#include "html_document.h"
#include "themes.h"
#include "exports.h"

class ChatEditBox;
class EmotsWalker;

enum EmoticonsStyle
{
	EMOTS_NONE,
	EMOTS_STATIC,
	EMOTS_ANIMATED
};

/**
	Menad�er emotikon�w
**/
class KADUAPI EmoticonsManager : public Themes, ConfigurationAwareObject
{
	Q_DISABLE_COPY(EmoticonsManager)

	/**
		Konstruktor tworzy obiekt oraz wyszukuje list� zestaw�w w katalogu
		$DATADIR/kadu/themes/emoticons
	**/
	EmoticonsManager();

	static EmoticonsManager *Instance;

	struct EmoticonsListItem
	{
		QString alias;
		QString doubleEscapedAlias; // sight, WebKit workaround
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
	static EmoticonsManager * instance();

	~EmoticonsManager();

	/**
		\brief Funkcja ustawia zestaw emotikonek na theme
		\param theme nazwa zestawu
		Funkcja ustawi zestaw je�li taki istnieje w przeciwnym wypadku
		ustawionym zestawem b�dzie "gadu-gadu"
	**/
	void setEmoticonsTheme(const QString &theme);

	/**
		Funkcja wstawia w text zamiast tag�w emotikonek odpowiednie emotikonki
		\param text dokument w kt�rym maj� by� zamienione tagi
		\param bgcolor kolor t�a emotikonki
		\param style styl emotikonki jaki ma by� wstawiony - domy�lnie jest do
		 aktualny zestaw
	**/
	void expandEmoticons(HtmlDocument &text, const QColor &bgcolor, EmoticonsStyle style);

	/**
		Funkcja zwraca ilo�� emotikonek w zestawie
	**/
	int selectorCount() const;

	/**
		Funkcja zwraca alias odpowiedniej emotikonki
		\param emot_num nr emotikonki z listy
		\return Zwracany jest pusty ci�g je�li nie znaleziono emotikonki w przeciwnym razie zwracany jest alias.
	**/
	QString selectorString(int emot_num) const;

	/**
		Funkcja zwraca �cie�k� do odpowiedniej animowanej emotikonki
		\param emot_num nr emotikonki z listy
		\return Zwracany jest pusty ci�g je�li nie znaleziono emotikonki w przeciwnym razie zwracana jest �cie�ka do emotikonki
	**/
	QString selectorAnimPath(int emot_num) const;

	/**
		Funkcja zwraca �cie�k� do odpowiedniej statycznej emotikonki
		\param emot_num nr emotikonki z listy
		\return Zwracany jest pusty ci�g je�li nie znaleziono emotikonki w przeciwnym razie zwracana jest �cie�ka do emotikonki
	**/
	QString selectorStaticPath(int emot_num) const;

	virtual void configurationUpdated();

};

/**
	Klasa s�u��ca do wyboru emotikonki z zestawu
**/
class EmoticonSelectorButton : public QLabel
{
	Q_OBJECT

	QString EmoticonString;
	QString AnimPath;
	QString StaticPath;

private slots:
	void buttonClicked();

protected:
	class MovieViewer;
	friend class MovieViewer;

	/**
		Funkcja obs�uguj�ca ruch kursora na obszarze emotikonki.
	**/
	void mouseMoveEvent(QMouseEvent *e);

public:
	/**
		Konstruktor tworz�cy przycisk z emotikonk�
		\param parent widget na kt�rym osadzona ma by� przycisk z emotikonk�
		\param emoticon_string nazwa kt�ra ma by� wy�wietlana po najechaniu na
		przycisk
		\param static_path �cie�ka do statycznej emotikonki
		\param anim_path �cie�ka do animowanej emotikonki
	**/
	EmoticonSelectorButton(const QString &emoticon_string, const QString &static_path, const QString &anim_path, QWidget *parent);
	
signals:
	/**
		Sygna� emitowany po naci�ni�ciu przycisku z emotikonk�
		przekazywana jest warto�� emoticon_string (przewa�nie jest to tag)
	**/
	void clicked(const QString &emoticon_string);

};

class EmoticonSelectorButton::MovieViewer : public QLabel
{
	Q_OBJECT

protected:
	bool event(QEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);

public:
	explicit MovieViewer(EmoticonSelectorButton *parent);

signals:
	void clicked();
};

/**
	Klasa wy�wietlaj�ca list� emotikonek z aktualnego zestawu.
**/
class EmoticonSelector : public QScrollArea
{
	Q_OBJECT

	ChatEditBox *callingwidget;

private slots:
	void iconClicked(const QString &emoticon_string);

protected:
	bool event(QEvent *e);

public:
	/**
		Konstruktor tworz�cy list� emotikonek.
		\param parent rodzic na kt�rym ma by� wy�wietlona lista
		\param name nazwa obiektu
		\param caller okno chat do ktorego ma by� wpisana wybrana emotikonka
	**/
	explicit EmoticonSelector(ChatEditBox *caller, QWidget *parent = 0);

public slots:
	/**
		Slot obs�uguj�cy poprawne wy�wietlenie listy emotikonek, wyr�wnanie do
		okna wywo�uj�cego.
	**/
	void alignTo(QWidget *w);

};

struct PrefixNode;
typedef QPair<QChar, PrefixNode *> Prefix;

struct PrefixNode
{
	int emotIndex;
	QList<Prefix> childs;
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
