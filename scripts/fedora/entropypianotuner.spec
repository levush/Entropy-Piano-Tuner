Summary:        Entropy Piano Tuner
Name:           entropypianotuner
Version:        1.0.5
Release:        1%{?dist}
License:        GPLv3
Group:          Applications/Productivity
URL:            http://www.entropy-tuner.org

# Source from gitlab (do the workaroud to get the correct version)
%global ARCHIVE archive.tar.gz?ref=v%{version}
%global OWNER entropytuner
%global PROJECT Entropy-Piano-Tuner
Source:         https://gitlab.com/%OWNER/%PROJECT/repository/%ARCHIVE #/archive.tar.gz

BuildRequires:  qt5-qtbase-devel, qt5-qtmultimedia-devel, fftw3-devel, desktop-file-utils, alsa-lib-devel
Requires:       qt5-qtbase, qt5-qtmultimedia, fftw3, alsa-lib

%description
This is a program for tuning your piano.

%prep
%setup -q -n %PROJECT.git

%{__cat} <<EOF >%{name}.desktop
[Desktop Entry]
Name=Entropy Piano Tuner
Comment=Tune your piano
Exec=entropypianotuner %f
Icon=entropypianotuner.png
Terminal=false
Type=Application
MimeType=application/ept;
Categories=Application;AudioVideo;Audio;
EOF

%build
qmake-qt5 -r 
make %{?_smp_mflags}


%install
INSTALL_ROOT=%{buildroot}%{_prefix} %make_install
%{__install} -d -m0755 %{buildroot}%{_datadir}/applications/
desktop-file-install \
    --add-category="AudioVideo"                \
    --delete-original                          \
    --dir %{buildroot}%{_datadir}/applications \
    %{name}.desktop

%post
%{_bindir}/update-desktop-database %{_datadir}/applications &>/dev/null || :
%{_bindir}/update-mime-database %{_datadir}/mime &/dev/null || :

%postun
%{_bindir}/update-desktop-database %{_datadir}/applications &>/dev/null || :
%{_bindir}/update-mime-database %{_datadir}/mime &/dev/null || :

%clean
%{__rm} -rf %{buildroot}

%files
%defattr(-, root, root, 0755)
%{_bindir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/pixmaps/%{name}.png
%{_datadir}/icons/application-ept.png
%{_datadir}/mime/packages/%{name}-mime.xml

%changelog
* Fri Jun 12 2015 Christoph Wick <info@entropy-tuner.org> - 1.0.4-0
- Initial package.
