# run this script to build and upload the package

# fetch the current master branch
git fetch upstream
git checkout upstream/master

# build and upload to the webserver
./debianBuild -ubp
