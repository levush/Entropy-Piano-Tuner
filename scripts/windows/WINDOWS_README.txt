Follow these instructions to use the script build system to automatically build the code and publish it to the webserver.

1. Preparation
==============

These steps have to be done once. They are required for setting up your user build environment.

- Copy the file windows_env.template.bat to windows_env.user.bat and open it in a text editor of your choice.
- Follow the instructions in the file to set up the paths correctly.
- copy the ssh-key for the webserver into cygwin:
  - open cygwin
  - cp $PATH_TO_KEY ~/.ssh
  - the script will automatically detect the key

2. Build and publish
====================

Always repeat this steps if you want to publish a new version.

- cd to the scripts/windows
- run windowsPublish.bat to build and publish the versions automatically to the webspace.
- You will be asked for the password for the ssh-key during the progress (twice).

3. Hints (optional)
====================

You can also run windowsPlatfromPublish.bat (the actual worker script) with a few options:
  -p [x86  | x64]     (required) to set the platform
  -c                             to clear any previous built
  -b                             to compile (build) the program
  -up                            update the packages in the repository
  -ud                            update the dependencies
  -ut                            update the translations
  -i                             create the installer
  -publish                       to publish the files to the webserver

4. Issues (optional)
====================

none, of-course!
