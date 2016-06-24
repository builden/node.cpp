# Node.cpp
* 用C++实现了Node.js的常用API接口
* 底层使用libuv, 支持事件驱动、非阻塞IO
* 实现了几个Node.js项目中被大量依赖的第三库
* 使用方式基本上和Node.js保持一致，开发过程中可直接参考Node.js的API文档
* 大量使用了C++11中的lambda，动态参数模版等特性，目前只在VS2013中编译通过

## 为何不直接在Windows客户端直接使用Node.js
1. 最新版本的Node.js不支持windows XP
2. Node.js会导致安装包增大10M多
3. 脚本在前端容易遭到破解

## 使用示例
```cpp
// fs
fs.readFile(filePath, [](const Buffer& buf) {
  ...
});
```

## 内置库
- [x] core-math  // js内置的math库
- [x] child_process
- [x] buffer
- [x] fs
- [ ] http
- [x] net
- [x] os
- [x] path
- [x] process
- [x] querystring
- [x] timers
- [x] url

## 第三方库
- [x] lodash
- [x] underscore.string
- [x] moment
- [x] iconv
- [x] file-url

## 自用库
- [x] keyboard-code-trans
  > 灵感来自于Electron
- [x] pe-cert
  * 用于检测程序的数字签名
- [x] shm
  * 共享内存
- [x] dll-util
  * 包装了对dll的一些常用操作，如获取dll的绝对路径
- [x] hooker
  * 包装了detours，方便hooker api
- [x] ini
  * 包装了ini文件的解析

## 工具类
- [x] singleton
  * 单例的包装类
- [x] std-redex-ex
  * 扩展了std::regex_replace
  > 支持formart回调

## 依赖
* [libuv](https://github.com/libuv/libuv)
* [gtest](https://github.com/google/googletest)
* [fmt](https://github.com/fmtlib/fmt)
  > 增加fmt::odebug接口，内部调用OutputDebugString，用于输出到dbgview
* [json11](https://github.com/dropbox/json11)
  > string改为wstring，默认采用宽字节
* [openssl](https://www.openssl.org/)
* [http_parser](https://github.com/nodejs/http-parser)
* [zlib](https://github.com/madler/zlib)
* [detours](http://research.microsoft.com/en-us/projects/detours/)