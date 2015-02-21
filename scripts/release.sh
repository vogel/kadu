#!/usr/bin/env bash

function update_version_file()
{
	VERSION=$1
	echo $VERSION > VERSION

	git commit -a -m "version: update to $VERSION"
}

function git_tag()
{
	VERSION=$1
	git tag $VERSION
	git push origin $VERSION
}

function create_package()
{
	VERSION=$1

	rm -rf package
	mkdir package
	pushd package
	../scripts/create-package.sh $VERSION
	popd
}

function build_binary()
{
	VERSION=$1

	rm -rf package-build
	mkdir package-build
	pushd package-build
	cmake ../package/kadu-$VERSION/
	make -j4
	popd
}

function write_forum_entry()
{
	VERSION=$1

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

	pushd package
	cat > forum <<-END
		Nowa stabilna wersja Kadu została właśnie wydana!

		Notka o wydaniu:
		 http://www.kadu.im/w/NotkaOWydaniu2
		 http://www.kadu.im/w/English:ReleaseNotes2

		Poprawione błędy:
		$CHANGELOG

		Instalator Windows i sumy kontrolne:
		 https://googledrive.com/host/0B2Jon_7ucnshOFg5NTNzazdmdk0/Kadu-$VERSION.exe
		 http://download.kadu.im/stable/windows/Kadu-$VERSION.exe
		 http://sourceforge.net/projects/kadu/files/kadu/$VERSION/Kadu-$VERSION.exe/download
		 http://download.kadu.im/stable/windows/Kadu-$VERSION.exe.md5
		 http://download.kadu.im/stable/windows/Kadu-$VERSION.exe.sha1

		Źródła i sumy kontrolne:
		 https://googledrive.com/host/0B2Jon_7ucnshOFg5NTNzazdmdk0/kadu-$VERSION.tar.bz2
		 http://download.kadu.im/stable/kadu-$VERSION.tar.bz2
		 http://sourceforge.net/projects/kadu/files/kadu/$VERSION/kadu-$VERSION.tar.bz2/download
		 http://download.kadu.im/stable/kadu-$VERSION.tar.bz2.md5
		 http://download.kadu.im/stable/kadu-$VERSION.tar.bz2.sha1

		Wtyczki zewnętrzne:
		 http://download.kadu.im/external-plugins/2.0/

		Błędy jak zawsze proszę zgłaszac na redmine: http://www.kadu.im/redmine/
END

	popd
}

function write_mailing_list_entry()
{
	VERSION=$1

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

	pushd package
	cat > mailing-list <<-END
		New stable version of Kadu 2 series is available.

		Release notes:
		 http://www.kadu.im/w/English:ReleaseNotes2
		 http://www.kadu.im/w/NotkaOWydaniu2

		Bugs fixed:
		$CHANGELOG

		Windows installer and checksums:
		 https://googledrive.com/host/0B2Jon_7ucnshOFg5NTNzazdmdk0/Kadu-$VERSION.exe
		 http://download.kadu.im/stable/windows/Kadu-$VERSION.exe
		 http://sourceforge.net/projects/kadu/files/kadu/$VERSION/Kadu-$VERSION.exe/download
		 http://download.kadu.im/stable/windows/Kadu-$VERSION.exe.md5
		 http://download.kadu.im/stable/windows/Kadu-$VERSION.exe.sha1

		Source code:
		 https://googledrive.com/host/0B2Jon_7ucnshOFg5NTNzazdmdk0/kadu-$VERSION.tar.bz2
		 http://download.kadu.im/stable/kadu-$VERSION.tar.bz2
		 http://sourceforge.net/projects/kadu/files/kadu/$VERSION/kadu-$VERSION.tar.bz2/download
		 http://download.kadu.im/stable/kadu-$VERSION.tar.bz2.md5
		 http://download.kadu.im/stable/kadu-$VERSION.tar.bz2.sha1

		External plugins:
		 http://download.kadu.im/external-plugins/2.0/

		Please report all found bugs in redmine: http://www.kadu.im/redmine/

		Regards,
		Rafał Malinowski
END

	popd
}

function write_pl_wiki_entry()
{
	VERSION=$1

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

	DATE=`date +"%d.%m.%Y"`

	pushd package
	cat > wiki-pl <<-END
		=== Kadu $VERSION wydane ''($DATE)'' ===
		----

		Nowa stabilna wersja Kadu została właśnie wydana!

		'''Pobierz''':
		:: Źródła dla systemu Linux ([[Instalacja_ze_źródeł|opis instalacji tej wersji Kadu]]):
		::: [https://googledrive.com/host/0B2Jon_7ucnshOFg5NTNzazdmdk0/kadu-$VERSION.tar.bz2 Pobierz źródła (Google Drive)]
		::: [http://sourceforge.net/projects/kadu/files/kadu/$VERSION/kadu-$VERSION.tar.bz2/download Pobierz źródła (serwer SourceForge.net)]
		::: [http://download.kadu.im/stable/kadu-$VERSION.tar.bz2 Pobierz źródła (serwer Kadu.im)]
		::: [http://download.kadu.im/stable/kadu-$VERSION.tar.bz2.md5 Suma MD5]
		::: [http://download.kadu.im/stable/kadu-$VERSION.tar.bz2.sha1 Suma SHA1]
		:: Instalator Windows:
		::: [https://googledrive.com/host/0B2Jon_7ucnshOFg5NTNzazdmdk0/Kadu-$VERSION.exe Pobierz Instalator Windows (Google Drive)]
		::: [http://sourceforge.net/projects/kadu/files/kadu/$VERSION/Kadu-$VERSION.exe/download Pobierz Instalator Windows (SourceForge.net)]
		::: [http://download.kadu.im/stable/windows/Kadu-$VERSION.exe Pobierz Instalator Windows (Kadu.im)]
		::: [http://download.kadu.im/stable/windows/Kadu-$VERSION.exe.md5 Suma MD5]
		::: [http://download.kadu.im/stable/windows/Kadu-$VERSION.exe.sha1 Suma SHA1]
		:: Wtyczki zewnętrzne:
		::: [http://download.kadu.im/external-plugins/2.0/ Źródła]

		Notka o wydaniu:
		* [[NotkaOWydaniu2|Wersja polska]]
		* [[English:ReleaseNotes2|Wersja angielska]]

		Lista zmian:
		$CHANGELOG

		Błędy prosimy zgłaszać w naszym systemie śledzenia błędów [http://www.kadu.im/redmine Redmine], a propozycje zmian na [http://www.kadu.im/forum forum] w dziale o odpowiednim tytule. Listę znany błędów zaplanowanych do poprawienia w przyszłych wersjach również można znaleźć w Redmine, w [http://www.kadu.im/redmine/projects/kadu/roadmap mapie dla projektu Kadu].

		<p>
		</p>
END

	popd
}

function write_en_wiki_entry()
{
	VERSION=$1

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

	DATE=`date +"%d/%m/%Y"`

	pushd package
	cat > wiki-en <<-END
		=== Kadu $VERSION has been released ''($DATE)'' ===
		----

		New stable version of Kadu 2 series is available.

		'''Download''':
		:: Source code:
		::: [https://googledrive.com/host/0B2Jon_7ucnshOFg5NTNzazdmdk0/kadu-$VERSION.tar.bz2 Download Sources (Google Drive)]
		::: [http://sourceforge.net/projects/kadu/files/kadu/$VERSION/kadu-$VERSION.tar.bz2/download Download Sources (SourceForge.net)]
		::: [http://download.kadu.im/stable/kadu-$VERSION.tar.bz2 Download Sources (Kadu.im)]
		::: [http://download.kadu.im/stable/kadu-$VERSION.tar.bz2.md5 MD5 Checksum]
		::: [http://download.kadu.im/stable/kadu-$VERSION.tar.bz2.sha1 SHA1 Checksum]
		:: Windows installer:
		::: [https://googledrive.com/host/0B2Jon_7ucnshOFg5NTNzazdmdk0/Kadu-$VERSION.exe Download Windows Installer (Google Drive)]
		::: [http://sourceforge.net/projects/kadu/files/kadu/$VERSION/Kadu-$VERSION.exe/download Download Windows Installer (SourceForge.net)]
		::: [http://download.kadu.im/stable/windows/Kadu-$VERSION.exe  Download Windows Installer (Kadu.im)]
		::: [http://download.kadu.im/stable/windows/Kadu-$VERSION.exe.md5 MD5 Checksum]
		::: [http://download.kadu.im/stable/windows/Kadu-$VERSION.exe.sha1 SHA1 Checksum]
		:: External plugins:
		::: [http://download.kadu.im/external-plugins/2.0/ Sources]

		Release notes:
		* [[English:ReleaseNotes2|English version]]
		* [[NotkaOWydaniu2|Polish version]]

		Fixed bugs:
		$CHANGELOG

		Please report all found issues in our [http://www.kadu.im/redmine Redmine] system.

		<p>
		</p>
END

	popd
}

VERSION=$1

update_version_file $VERSION
git_tag $VERSION
create_package $VERSION
build_binary $VERSION
write_forum_entry $VERSION
write_mailing_list_entry $VERSION
write_pl_wiki_entry $VERSION
write_en_wiki_entry $VERSION
