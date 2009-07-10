#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <QStringList>
#include <QMap>

#include "configuration/configuration-aware-object.h"
#include "gui/windows/main-configuration-window.h"

class AspellSpeller;
class AspellConfig;
class ChatWidget;

class QListWidget;
class QListWidgetItem;

class SpellChecker : public ConfigurationUiHandler, ConfigurationAwareObject
{
    Q_OBJECT

	typedef QMap<QString, AspellSpeller *> Checkers;
	Checkers checkers;
	AspellConfig *spellConfig;

	QListWidget *availList;
	QListWidget *checkList;

	void createDefaultConfiguration();

private slots:
	void configurationWindowApplied();

protected:
	virtual void configurationUpdated();

public:
	SpellChecker();
	virtual ~SpellChecker();
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	QStringList notCheckedLanguages();
	QStringList checkedLanguages();
	bool addCheckedLang(QString& name);
	void removeCheckedLang(QString& name);
	void buildMarkTag();
	bool buildCheckers();
	bool checkWord(QString word);

public slots:
	void chatCreated(ChatWidget *chatWidget);
	void configForward();
	void configBackward();
	void configForward2(QListWidgetItem *item);
	void configBackward2(QListWidgetItem *item);

};

extern SpellChecker* spellcheck;

#endif
