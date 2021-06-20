#!/bin/bash

set -x #echo commands

root=`git rev-parse --show-toplevel`

if [ ! -d "$root" ]; then
 echo "Cannot find root git folder $root. Make sure you run the script from within the git repository folder."
 exit
fi

pushd $root/..
tar -czvf "/tmp/maild.tar.gz" maild
popd
mv /tmp/maild.tar.gz .


buildah bud  -f Dockerfile.build  -t build-deb:build
podman container create --name extract build-deb:build
podman container cp extract:/tmp/workdir/maild/build/maild.rpm ./
podman container rm -f extract

podman rmi $(podman images --filter "dangling=true" -q --no-trunc)
podman image prune -fa

rm maild.tar.gz

