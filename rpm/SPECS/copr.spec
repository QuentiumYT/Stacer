Name:           stacer
Version:        1.8.0
Release:        1%{?dist}
Summary:        Linux system optimizer and monitoring

License:        GPL-3.0
URL:            https://github.com/QuentiumYT/Stacer
Source0:        %{url}/archive/v%{version}/%{name}-%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  desktop-file-utils

%if 0%{?suse_version}
BuildRequires:  gcc15-c++
BuildRequires:  ninja
BuildRequires:  update-desktop-files
BuildRequires:  appstream-glib
%else
BuildRequires:  gcc-c++
BuildRequires:  ninja-build
BuildRequires:  libappstream-glib
%endif

BuildRequires:  cmake(Qt6Core)
BuildRequires:  cmake(Qt6Gui)
BuildRequires:  cmake(Qt6Widgets)
BuildRequires:  cmake(Qt6LinguistTools)
BuildRequires:  cmake(Qt6Charts)
BuildRequires:  cmake(Qt6Svg)
BuildRequires:  cmake(Qt6Concurrent)
BuildRequires:  cmake(Qt6Network)

Requires:       hicolor-icon-theme

%if 0%{?suse_version}
Requires:       qt6-charts
Requires:       qt6-svg
%else
Requires:       qt6-qtcharts%{?_isa}
Requires:       qt6-qtsvg%{?_isa}
%endif

%description
Monitor your system (CPU, memory, disk) in a graphical application (Qt).
Change and monitor your services. Summarizes basic system information and
can show network download/upload speeds/totals.

%prep
%autosetup -n Stacer-%{version}

%build
%if 0%{?suse_version}
export CXX=/usr/bin/g++-15

cmake -G Ninja -S . -B build \
    -DCMAKE_INSTALL_PREFIX=%{_prefix} \
    -DCMAKE_BUILD_TYPE=Release

cmake --build build -j $(nproc)
%else
%cmake -G Ninja \
    -DCMAKE_INSTALL_PREFIX=%{_prefix} \
    -DCMAKE_BUILD_TYPE=Release

%cmake_build
%endif

# Build translations
%if 0%{?suse_version}
%{_libdir}/qt6/bin/lrelease %{name}/%{name}.pro
%else
lrelease-qt6 %{name}/%{name}.pro
%endif

%install
%if 0%{?suse_version}
DESTDIR=%{buildroot} cmake --install build
%suse_update_desktop_file -r %{name} System Monitor
%else
%cmake_install
%endif

%find_lang %{name} --with-qt

%check
%if ! (0%{?rhel} <= 9)
appstream-util validate-relax --nonet %{buildroot}%{_datadir}/metainfo/*.metainfo.xml || true
%endif
desktop-file-validate %{buildroot}%{_datadir}/applications/*.desktop

%files -f %{name}.lang
%license LICENSE
%doc README.md
%{_bindir}/%{name}
%{_datadir}/applications/*.desktop
%{_datadir}/icons/hicolor/*/*/*.png
%{_datadir}/icons/hicolor/scalable/apps/*.svg
%{_datadir}/metainfo/fr.quentium.%{name}.metainfo.xml

# Translations files which rpm macros cant handle
%dir %{_datadir}/%{name}/translations/
%{_datadir}/%{name}/translations/stacer_zh-cn.qm
%{_datadir}/%{name}/translations/stacer_zh-tw.qm

%changelog
* Thu Sep 17 2026 Quentin Lienhardt <pro@quentium.fr> - 1.8.0-1
- Initial package republish
