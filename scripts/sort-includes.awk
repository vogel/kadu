BEGIN {
	# declare array
	split("", include_group)
}

/\#include/ {
	include_group[length(include_group)] = $0
}

!/\#include/ {
	if (length(include_group) > 0) {
		# new arrays
		split("", current_dir_include_group)
		split("", other_dir_include_group)

		for (x in include_group)
			if (include_group[x] ~ /\//)
				other_dir_include_group[length(other_dir_include_group)] = include_group[x]
			else
				current_dir_include_group[length(current_dir_include_group)] = include_group[x]

		n = asort(other_dir_include_group)
		for (i = 1; i <= n; i++)
			print(other_dir_include_group[i])

		n = asort(current_dir_include_group)
		for (i = 1; i <= n; i++)
			print(current_dir_include_group[i])
	}

	# clear array
	split("", include_group)

	print $0
}
