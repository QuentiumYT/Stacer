Name:           stacer
Version:        1.8.0
Release:        1%{?dist}
Summary:        Linux system optimizer and monitoring

License:        GPL-3.0
URL:            https://stacer.quentium.fr/
Source0:        %{name}-%{version}.tar.gz

BuildArch:      noarch
Requires:       qt6-qtbase qt6-qtbase-gui qt6-qtcharts qt6-qtsvg glibc systemd curl

%description
Monitor your system (CPU, memory, disk) in a graphical application (Qt).
Change and monitor your services. Summarizes basic system information and
can show network download/upload speeds/totals.

%prep
%setup -q

# WARNING: Strip doesn't work for building rpm
%define __brp_strip /bin/true
%define __brp_strip_static_archive /bin/true

%build
cmake -S . -B build \
    -DCMAKE_INSTALL_PREFIX=%{_prefix} \
    -DCMAKE_BUILD_TYPE=Release

cmake --build build -j $(nproc)

%install
rm -rf %{buildroot}

DESTDIR=%{buildroot} cmake --install build

%files
%license LICENSE
%doc README.md debian/changelog debian/copyright
%{_bindir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
%{_datadir}/icons/hicolor/scalable/apps/%{name}.svg
%{_datadir}/metainfo/fr.quentium.%{name}.metainfo.xml
%{_datadir}/%{name}/translations/*.qm

%changelog
