#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <QtCore/QString>
#include <QtCore/QMap>

#include "configuration/configuration-aware-object.h"
#include "gui/windows/main-configuration-window.h"

class QListWidget;
class QListWidgetItem;

class AspellSpeller;
class AspellConfig;
class ChatWidget;

class SpellChecker : public ConfigurationUiHandler, ConfigurationAwareObject
{
    Q_OBJECT

	typedef QMap<QString, AspellSpeller *> Checkers;
	Checkers MyCheckers;
	AspellConfig *SpellConfig;

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
	bool buildCheckers();
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
