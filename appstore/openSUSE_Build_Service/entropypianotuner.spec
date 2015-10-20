Summary:        Entropy Piano Tuner
Name:           entropypianotuner
Version: 1.1.3
Release:        1%{?dist}
License:        GPLv3
Group:          Applications/Productivity
URL:            http://www.piano-tuner.org

# Source from gitlab
# Change the hash according to the current master branch
# Workaround for source containing a '?' (commend at end)
%global COMMIT  046bfc35f8ab3a7343aea6515730b037a33e9c16
%global BRANCH  master
%global ARCHIVE archive.tar.gz?ref=%BRANCH
%global OWNER entropytuner
%global PROJECT Entropy-Piano-Tuner
Source:         https://gitlab.com/%OWNER/%PROJECT/repository/%ARCHIVE #/archive.tar.gz

%if 0%{?fedora}
BuildRequires:  qt5-qtbase-devel, qt5-qtmultimedia-devel, fftw3-devel, desktop-file-utils, alsa-lib-devel
Requires:       qt5-qtbase, qt5-qtmultimedia, fftw3, alsa-lib
%endif
%if 0%{?suse_version}
BuildRequires:  libatomic1, libqt5-qtbase-devel, libqt5-qtmultimedia-devel, fftw-devel, update-desktop-files, alsa-devel
Requires:       libqt5-qtbase, libQt5Multimedia5, fftw3, alsa
%endif

%description
This is a program for tuning your piano.

%prep
%setup -q -n %PROJECT-%BRANCH-%COMMIT

%{__cat} <<EOF >%{name}.desktop
[Desktop Entry]
Name=Entropy Piano Tuner
Comment=Tune your piano
Exec=entropypianotuner %f
Icon=entropypianotuner
Terminal=false
Type=Application
MimeType=application/ept;
Categories=Education;Music;
EOF

%build
qmake-qt5 -r 
make %{?_smp_mflags}


%install
INSTALL_ROOT=%{buildroot}%{_prefix} %make_install
%{__install} -d -m0755 %{buildroot}%{_datadir}/applications/
desktop-file-install \
    --delete-original                          \
    --dir %{buildroot}%{_datadir}/applications \
    %{name}.desktop

%post
%desktop_database_post

%postun
%desktop_database_postun

%clean
%{__rm} -rf %{buildroot}

%files
%dir %{_datadir}/icons
%dir %{_datadir}/icons/hicolor
%dir %{_datadir}/icons/hicolor/256x256
%dir %{_datadir}/icons/hicolor/256x256/mimetypes
%defattr(-, root, root, 0755)
%{_bindir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/pixmaps/%{name}.png
%{_datadir}/icons/hicolor/256x256/mimetypes/application-ept.png
%{_datadir}/mime/packages/%{name}-mime.xml

%changelog
* Fri Jun 12 2015 Christoph Wick <info@entropy-tuner.org> - 1.0.4-0
- Initial package.
