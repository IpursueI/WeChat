#一个基于c++的微信后台
##主要用于练习c++，以及cgi编程
改后台绑定的微信公众号是：captainHuang，可关注并测试效果
编译命令:g++ -std=c++11 -o WeChat.cgi WeChat.cpp CheckSignature.cpp -lcgicc -lssl -lcrypto -ltinyxml
目前使用的第三方库包含如下:
- Cgicc
- OpenSSL
- TinyXml

