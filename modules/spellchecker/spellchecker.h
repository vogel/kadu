#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <QtCore/QString>
#include <QtCore/QMap>

#include "configuration/configuration-aware-object.h"
#include "gui/windows/main-configuration-window.h"

class QListWidget;
class QListWidgetItem;

class ChatWidget;

#ifdef HAVE_ASPELL
class AspellSpeller;
class AspellConfig;
#else
namespace enchant
{
	class Dict;
}
#endif

class SpellChecker : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT

public:
#ifdef HAVE_ASPELL
	typedef QMap<QString, AspellSpeller *> Checkers;
#else
	typedef QMap<QString, enchant::Dict *> Checkers;
#endif

private:
#ifdef HAVE_ASPELL
	AspellConfig *SpellConfig;
#endif

	Checkers MyCheckers;

	QListWidget *AvailableLanguagesList;
	QListWidget *CheckedLanguagesList;

	void createDefaultConfiguration();

private slots:
	void configurationWindowApplied();

protected:
	virtual void configurationUpdated();

public:
	SpellChecker();
	virtual ~SpellChecker();

	QStringList notCheckedLanguages();
	QStringList checkedLanguages();
	bool addCheckedLang(const QString &name);
	void removeCheckedLang(const QString &name);
	void buildMarkTag();
	void buildCheckers();
	bool checkWord(const QString &word);

public slots:
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	void chatCreated(ChatWidget *chatWidget);
	void configForward();
	void configBackward();
	void configForward2(QListWidgetItem *item);
	void configBackward2(QListWidgetItem *item);

};

extern SpellChecker *spellcheck;

#endif // SPELLCHECKER_H
