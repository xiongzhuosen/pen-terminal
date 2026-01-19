mkdir -p ~/extract
for pkg in ~/deb/*.deb ~/deb/*.udeb; do
  dpkg-deb --fsys-tarfile "$pkg" | tar -xpf - -C ~/extract --overwrite
done