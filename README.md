C语言搞API,妥妥的二

cd curl
./configure
make -j8
sudo make install
gcc -o test cJSON/cJson.c test.c -lcurl -lm
