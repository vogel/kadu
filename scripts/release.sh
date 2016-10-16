#!/usr/bin/env bash

function expand_version()
{
	if [[ $VERSION =~ ^[0-9]+\.[0-9]+(\.[0-9]+)?$ ]];
	then
		export BRANCH=STABLE
		export EN_TEST=stable
		export PL_TEST=stabilna
	else
		export BRANCH=UNSTABLE
		export EN_TEST=test
		export PL_TEST=testowa
	fi

	[[ $VERSION =~ ^([0-9]+)\..+$ ]]
	export MAIN=${BASH_REMATCH[1]}
}

function prepare_links()
{
	export SOURCE_SF=https://sourceforge.net/projects/kadu/files/kadu/$VERSION/kadu-$VERSION.tar.bz2/download
	export SOURCE_KADU=http://download.kadu.im/unstable/kadu-$VERSION.tar.bz2
	export SOURCE_KADU_MD5=http://download.kadu.im/unstable/kadu-$VERSION.tar.bz2.md5
	export SOURCE_KADU_SHA1=http://download.kadu.im/unstable/kadu-$VERSION.tar.bz2.sha1
	export EXE_SF=https://sourceforge.net/projects/kadu/files/kadu/$VERSION/Kadu-$VERSION.x86.exe/download
	export EXE_KADU=http://download.kadu.im/unstable/windows/Kadu-$VERSION.x86.exe
	export EXE_KADU_MD5=http://download.kadu.im/unstable/windows/Kadu-$VERSION.x86.exe.md5
	export EXE_KADU_SHA1=http://download.kadu.im/unstable/windows/Kadu-$VERSION.x86.exe.sha1
}

function read_changelog()
{
	CHANGELOG=""
	while read line; do
		if [[ -z "$line" ]]; then
			break
		fi
		if [[ "$line" =~ ^\*.* ]]; then
			if [[ -z "$CHANGELOG" ]]; then
				CHANGELOG=$line
			else
				CHANGELOG="$CHANGELOG
$line"
			fi
		fi
	done < ChangeLog

	export CHANGELOG=$CHANGELOG
}

function update_version_file()
{
	echo $VERSION > VERSION

	git commit -a -m "version: update to $VERSION"
}

function git_tag()
{
	git tag $VERSION
	git push origin $VERSION
}

function create_package()
{
	pushd package
	../scripts/create-package.sh $VERSION
	popd
}

function build_binary()
{
	rm -rf package-build
	mkdir package-build
	pushd package-build
	cmake ../package/kadu-$VERSION/
	make -j4
	popd
}

function write_forum_entry()
{
	pushd package
	cat > forum <<-END
		Nowa $PL_TEST wersja Kadu z serii $MAIN została właśnie wydana!

		Notka o wydaniu:
		 http://www.kadu.im/w/NotkaOWydaniu$MAIN
		 http://www.kadu.im/w/English:ReleaseNotes$MAIN

		Zmiany:
		$CHANGELOG

		Instalator Windows i sumy kontrolne:
		 $EXE_SF
		 $EXE_KADU
		 $EXE_KADU_MD5
		 $EXE_KADU_SHA1

		Źródła i sumy kontrolne:
		 $SOURCE_SF
		 $SOURCE_KADU
		 $SOURCE_KADU_MD5
		 $SOURCE_KADU_SHA1

		Przed instalacją zalecane jest zrobienie kopii katalogu z danymi ~/.kadu lub C:\Users\..\AppData\Roaming\Kadu.
		Błędy jak zawsze proszę zgłaszac na redmine: http://www.kadu.im/redmine/
END

	popd
}

function write_mailing_list_entry()
{
	pushd package
	cat > mailing-list <<-END
		New $EN_TEST version of Kadu $MAIN series is available.

		Release notes:
		 http://www.kadu.im/w/English:ReleaseNotes$MAIN
		 http://www.kadu.im/w/NotkaOWydaniu$MAIN

		Changes:
		$CHANGELOG

		Windows installer and checksums:
		 $EXE_SF
		 $EXE_KADU
		 $EXE_KADU_MD5
		 $EXE_KADU_SHA1

		Source code:
		 $SOURCE_SF
		 $SOURCE_KADU
		 $SOURCE_KADU_MD5
		 $SOURCE_KADU_SHA1

		Please create backup of data directory ~/.kadu or C:\Users\..\AppData\Roaming\Kadu before installing.
		Please report all found bugs in redmine: http://www.kadu.im/redmine/

		Regards,
		Rafał Malinowski
END

	popd
}

function write_pl_wiki_entry()
{
	DATE=`date +"%d.%m.%Y"`

	pushd package
	cat > wiki-pl <<-END
		=== Kadu $VERSION wydane ''($DATE)'' ===
		----

		Nowa $PL_TEST wersja Kadu z serii $MAIN została właśnie wydana!

		Notka o wydaniu:
		* [[NotkaOWydaniu$MAIN|Wersja polska]]
		* [[English:ReleaseNotes$MAIN|Wersja angielska]]

		'''Pobierz''':
		:: Źródła dla systemu Linux ([[Instalacja_ze_źródeł|opis instalacji tej wersji Kadu]]):
		::: [$SOURCE_SF Pobierz źródła (serwer SourceForge.net)]
		::: [$SOURCE_KADU Pobierz źródła (serwer Kadu.im)]
		::: [$SOURCE_KADU_MD5 Suma MD5]
		::: [$SOURCE_KADU_SHA1 Suma SHA1]
		:: Instalator Windows:
		::: [$EXE_SF Pobierz Instalator Windows (SourceForge.net)]
		::: [$EXE_KADU Pobierz Instalator Windows (Kadu.im)]
		::: [$EXE_KADU_MD5 Suma MD5]
		::: [$EXE_KADU_SHA1 Suma SHA1]

		Zmiany:
		$CHANGELOG

		Przed instalacją zalecane jest zrobienie kopii katalogu z danymi ~/.kadu lub C:\Users\..\AppData\Roaming\Kadu.

		Błędy prosimy zgłaszać w naszym systemie śledzenia błędów [http://www.kadu.im/redmine Redmine], a propozycje zmian na [http://www.kadu.im/forum forum] w dziale o odpowiednim tytule. Listę znany błędów zaplanowanych do poprawienia w przyszłych wersjach również można znaleźć w Redmine, w [http://www.kadu.im/redmine/projects/kadu/roadmap mapie dla projektu Kadu].

		<p>
		</p>
END

	popd
}

function write_en_wiki_entry()
{
	DATE=`date +"%d/%m/%Y"`

	pushd package
	cat > wiki-en <<-END
		=== Kadu $VERSION has been released ''($DATE)'' ===
		----

		New $EN_TEST version of Kadu $MAIN series is available.

		Release notes:
		* [[English:ReleaseNotes$MAIN|English version]]
		* [[NotkaOWydaniu$MAIN|Polish version]]

		'''Download''':
		:: Source code:
		::: [$SOURCE_SF Download Sources (SourceForge.net)]
		::: [$SOURCE_KADU Download Sources (Kadu.im)]
		::: [$SOURCE_KADU_MD5 MD5 Checksum]
		::: [$SOURCE_KADU_SHA1 SHA1 Checksum]
		:: Windows installer:
		::: [$EXE_SF Download Windows Installer (SourceForge.net)]
		::: [$EXE_KADU  Download Windows Installer (Kadu.im)]
		::: [$EXE_KADU_MD5 MD5 Checksum]
		::: [$EXE_KADU_SHA1 SHA1 Checksum]

		Changes:
		$CHANGELOG

		Please create backup of data directory ~/.kadu or C:\Users\..\AppData\Roaming\Kadu before installing.

		Please report all found issues in our [http://www.kadu.im/redmine Redmine] system.

		<p>
		</p>
END

	popd
}

export VERSION=$1

rm -rf package
mkdir package

expand_version
prepare_links
read_changelog
update_version_file
git_tag
create_package
build_binary
write_forum_entry
write_mailing_list_entry
write_pl_wiki_entry
write_en_wiki_entry
