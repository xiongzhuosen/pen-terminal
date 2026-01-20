wget https://developer.arm.com/-/media/Files/downloads/gnu/15.2.rel1/binrel/arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-linux-gnueabihf.tar.xz
sudo apt update
sudo apt install build-essential cmake megatools -y
tar -xvf arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-linux-gnueabihf.tar.xz
mv arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-linux-gnueabihf arm-none-linux-gnueabihf
cd /root/ssh-vnc-proj/deps
bash library.sh
cd ..
bash build_so.sh
rm -rf build build_log
