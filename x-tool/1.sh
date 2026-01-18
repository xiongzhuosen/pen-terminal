cat part_* > a.tar.xz
sudo apt update
sudo apt install build-essential cmake -y
tar -zxvf a.tar.gz
mv arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-linux-gnueabihf arm-none-linux-gnueabihf