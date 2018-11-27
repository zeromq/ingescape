# This spec file work with gitlab job
# So it do not build library from source
Name:           libingescape
Version:        %{_VERSION_I}
Release:        1%{?dist}
Group:          System/Libraries
Summary:        The ingescape library

License:        Not defined yet
URL:            https://ingescape.com/
Source0:        empty

Requires:       zeromq
Requires:       czmq
Requires:       zyre

%description
The %{name} library


%package        devel
Group:          System/Libraries
Summary:        Development files for %{name}
Requires:       %{name}%{?_isa} = %{version}-%{release}
Requires:       zeromq-devel
Requires:       czmq-devel
Requires:       zyre-devel

%description    devel
The %{name}-devel package contains header files for
developing applications that use %{name}.

%prep
# -c Create directories
# -q Run quietly with minimal output
# -T Do Not Perform Default Archive Unpacking
# -D Do Not Delete Directory Before Unpacking Sources
%setup -c -q -T -D

%build
mkdir -p %{buildroot}/usr/local/lib
mkdir -p %{buildroot}/usr/local/include/ingescape


%install
cp -d %{_INGESCAPE_SOURCE}/linux-x86/libingescape.la %{buildroot}/usr/local/lib/
cp -d %{_INGESCAPE_SOURCE}/linux-x86/libingescape.so* %{buildroot}/usr/local/lib/
cp -d %{_INGESCAPE_SOURCE}/src/include/ingescape.h %{buildroot}/usr/local/include/ingescape/


%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%defattr(-,root,root)
/usr/local/lib/libingescape.la
/usr/local/lib/libingescape.so*

%files devel
%defattr(-,root,root)
/usr/local/include/*


%changelog
* Tue Nov 27 2018 Esteveny <esteveny@ingenuity.io>
- RPM creation
