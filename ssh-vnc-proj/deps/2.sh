for d in *.deb; do
    dpkg-deb --extract "$d" .
done
