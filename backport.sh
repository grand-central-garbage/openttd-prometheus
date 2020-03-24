#!/usr/bin/env bash

set -euxo pipefail

git checkout master

git remote rm upstream
git remote add upstream https://github.com/OpenTTD/OpenTTD.git
git remote update upstream

commits_to_backport=$(git cherry upstream/master | grep "^\+" | cut -d' ' -f2)

target_branches="1.9 1.10"
for branch in $target_branches; do
    git branch -D "release/${branch}" || true
    git checkout --track "upstream/release/${branch}"

    for commit in $commits_to_backport; do
        git cherry-pick -x "${commit}"
    done

    tag="${branch}.0"
    git push origin ":refs/tags/${tag}"
    git tag -f "${tag}"
    git push --tags -f origin "release/${branch}"
done

git push -f origin master
