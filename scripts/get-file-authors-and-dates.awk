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
		item = sprintf("%s %s", years, author)
		printf sprintf(" *   Copyright %-60s *\n", item)
	}
}
