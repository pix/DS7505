#!/bin/bash

die() {
    echo "$@"
    exit 1
}

[ -z "$(which rsync)" ] && die "rsync isn't available"
[ -z "$(which doxygen)" ] && die "doxygen isn't available"
[ -z "$(which git)" ] && die "doxygen isn't available"

REMOTE=$(git remote show -n origin 2>/dev/null| grep "Push  URL:" | cut -d: -f2-)
[ -z "${REMOTE}" ] && die "No origin remote defined"

S="$(git rev-parse --show-toplevel)"
[ -d "${S}/tmp" ] && die "Please remove ${S}/tmp"

mkdir "${S}/tmp"
cd "${S}/tmp"
echo "Cloning: $REMOTE"
git clone $REMOTE -b gh-pages gh-pages || exit 1
cd gh-pages
git rm -r . || echo 'No files to remove'

cd "${S}"
doxygen
rsync -a --exclude=.git --delete-before "${S}/doc/html/" "${S}/tmp/gh-pages"
cd "${S}/tmp/gh-pages"
git add .
git commit -m "Docs updated: $(date)"
# git push origin gh-pages

