BEGIN {
	FS = "\t"
}

{
	if ($2 in AUTHORS) {
		AUTHORS[$2] = AUTHORS[$2] ", " substr($1, 0, 4)
	} else {
		AUTHORS[$2] = substr($1, 0, 4)
	}
}

END {
	for (author in AUTHORS) {
		years = AUTHORS[author]
		printf "Copyright %s %s\n", years, author
	}
}
