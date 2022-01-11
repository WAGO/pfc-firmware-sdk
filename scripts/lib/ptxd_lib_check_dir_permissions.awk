#!/usr/bin/awk -f

BEGIN {
	FS = "\x1F";
}

function check(path, perm, implicit) {
	if ((path in perms) && (perms[path] != perm)) {
		if (implicit && (pkg == names[path]))
			return;
		print("\nIncompatible ownership or permissions for '" path "':")
		print(names[path] ": " perms[path] (imp[path] ? " (implicit from " imp[path]")" : ""))
		print(pkg ": " perm (implicit ? " (implicit)" : ""))
		print("\nOne of these packages must be fixed!\n")
		exit 1
	}
	if (!(path in perms)) {
		names[path] = pkg
		perms[path] = perm
		imp[path] = implicit
	}
}

FNR == 1 {
	for (path in dirs)
		check(path, dirs[path], imps[path])
	pkg = gensub(/.*\/(.*).perms/, "\\1", 1, FILENAME)
	delete dirs
	delete imps
}

function push_parents(base) {
	path = base
	while (1) {
		path = gensub(/\/[^/]*$/,"",1,path)
		if (path == "")
			break;
		if (!(path in dirs)) {
			dirs[path] = "0.0 0755"
			imps[path] = base
		}
	}
}

$1 ~ "d" {
	path = gensub(/\/$/,"",1,$2)
	perm = $3 "." $4 " " sprintf("%04o", strtonum("0" $5))
	dirs[path] = perm
	imps[path] = ""
	push_parents(path)
}

$1 ~ "f" {
	path = $2
	push_parents(path)
}
