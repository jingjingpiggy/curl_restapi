C语言搞API,妥妥的二

cd curl
./configure
make -j8
sudo make install
gcc -o test cJSON/cJson.c test.c -lcurl -lm

windows 的IDE比较麻烦:
添加libcurl工程
1. General Windows SDK Version: 10.0.17134.0
2. C/C++ General: Additional Include Directories: libcurl 头文件路径; Preprocessor Definitions: + _CRT_SECURE_NO_WARNINGS; Code Generation: Muti-threaded(/MT)"静态库"; 
3. Linker: Additional Library Directories: libcurl lib库路径; Input: Ignore Specific Default Libraries: libcmt.lib
4. 补充，在windows IDE中如果只能看到curl和libcurl，那需要通过右键点击Solution ‘OSInstall’->Add->New Project，然后把代码挪到这里面就ok了。

