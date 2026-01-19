wget https://developer.arm.com/-/media/Files/downloads/gnu/15.2.rel1/binrel/arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-linux-gnueabihf.tar.xz
sudo apt update
sudo apt install build-essential cmake -y
tar -zxvf arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-linux-gnueabihf.tar.xz
mv arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-linux-gnueabihf arm-none-linux-gnueabihf