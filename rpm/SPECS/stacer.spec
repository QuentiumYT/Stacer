Name:           stacer
Version:        1.6.0
Release:        1%{?dist}
Summary:        Linux system optimizer and monitoring

License:        GPLv3
URL:            https://stacer.quentium.fr/
Source0:        %{name}-%{version}.tar.gz

BuildArch:      x86_64
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
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j $(nproc)

%install
rm -rf %{buildroot}

# Install binary
install -Dm755 build/stacer/stacer %{buildroot}/usr/bin/stacer

# Install .desktop file
install -Dm644 desktop/stacer.desktop %{buildroot}/usr/share/applications/stacer.desktop

# Install metainfo file
install -Dm644 build/desktop/fr.quentium.stacer.metainfo.xml %{buildroot}/usr/share/metainfo/fr.quentium.stacer.metainfo.xml

# Install icon sizes
for size in 16 32 64 128 256 512 1024; do
    install -Dm644 icons/hicolor/${size}x${size}/apps/stacer.png \
        %{buildroot}/usr/share/icons/hicolor/${size}x${size}/apps/stacer.png
done
install -Dm644 icons/hicolor/scalable/apps/stacer.svg \
    %{buildroot}/usr/share/icons/hicolor/scalable/apps/stacer.svg

# Install translation files
mkdir -p %{buildroot}/usr/share/qt6/translations
install -Dm644 build/stacer/translations/*.qm %{buildroot}/usr/share/qt6/translations/

# Install README doc
install -Dm644 README.md %{buildroot}/usr/share/doc/stacer/README.md

# Install changelog
install -Dm644 debian/changelog %{buildroot}/usr/share/doc/stacer/changelog

# Install copyright
install -Dm644 LICENSE %{buildroot}/usr/share/doc/stacer/copyright

%files
%license
%doc
/usr/bin/stacer
/usr/share/applications/stacer.desktop
/usr/share/doc/stacer/README.md
/usr/share/doc/stacer/changelog
/usr/share/doc/stacer/copyright
/usr/share/icons/hicolor/*/apps/stacer.png
/usr/share/icons/hicolor/scalable/apps/stacer.svg
/usr/share/metainfo/fr.quentium.stacer.metainfo.xml
/usr/share/qt6/translations/*.qm

%changelog
