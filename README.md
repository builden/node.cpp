# Node.cpp
* 用C++实现了Node.js的常用API接口
* 底层使用libuv, 支持事件驱动、非阻塞IO
* 实现了几个Node.js项目中被大量依赖的第三库
* 使用方式基本上和Node.js保持一致，开发过程中可直接参考Node.js的API文档
* 大量使用了C++11中的lambda，动态参数模版等特性，目前只在VS2015中编译通过

## 为何不直接在Windows客户端直接使用Node.js
* 最新的Node.js已经不支持xp系统
* Node.js目前在前端容易遭到破解
* Node.js本身的尺寸较大，安装包在11M左右

## 使用示例
```cpp
// fs
fs.readFile(filePath, [](const Buffer& buf) {
  ...
});
```

## 内置库
- [x] buffer
- [x] core-math  // js内置的math库
- [x] child_process
- [x] crypto
- [x] dns
- [x] fs
- [ ] http
- [x] net
- [x] os
- [ ] stream
- [x] path
- [x] process
- [x] querystring
- [ ] thread
- [x] timers
- [x] url
- [x] zlib

## 第三方库
- [x] lodash
- [x] underscore.string
- [x] moment
- [x] iconv
- [x] file-url
- [ ] commander
- [ ] later
- [ ] semver
- [x] winreg

## 自用库
- [x] keyboard-code-trans
  > 灵感来自于Electron
- [x] pe-cert
  * 用于检测程序的数字签名
- [x] shm
  * 共享内存
- [x] dll-util
  * 包装了对dll的一些常用操作，如获取dll的绝对路径
- [x] detours-hooker
  * 包装了detours，方便hooker api，启动进程前注入dll
- [x] ini
  * 包装了ini文件的解析

## 工具类
- [x] singleton
  * 单例的包装类
- [x] std-redex-ex
  * 扩展了std::regex_replace
  > 支持formart回调

## 初始化编译
1. 需提前编译Detours.lib放入
   > nmake
2. 需安装CMake，并为glog3生成工程
   > cmake -G "Visual Studio 14"
3. 在deps目录，下载各个模块
```bash
git clone -b 1.2 https://github.com/KjellKod/g3log.git
git clone -b v2.7.0 https://github.com/nodejs/http-parser.git
git clone -b 1.2.8 https://github.com/madler/zlib.git
```

## 工程设置
* Output Directory: $(SolutionDir)bin\$(Configuration)\
* Intermediate Directory: $(SolutionDir)bin\obj\$(Configuration)\$(ProjectName)\

## 依赖
* [libuv](https://github.com/libuv/libuv)
* [gtest](https://github.com/google/googletest)
* [fmt](https://github.com/fmtlib/fmt)
  > 增加fmt::odebug接口，内部调用OutputDebugString，用于输出到dbgview
* [json](https://github.com/nlohmann/json)
  > c++11，只支持VS2015，不支持VS2013
* [openssl](https://www.openssl.org/)
* [http_parser](https://github.com/nodejs/http-parser)
* [zlib](https://github.com/madler/zlib)
* [detours](http://research.microsoft.com/en-us/projects/detours/)
* [simpleini](https://github.com/brofield/simpleini)
* [g3log](https://github.com/KjellKod/g3log)

## 参考
* [node.native](https://github.com/d5/node.native)
* [uvbook中文文档](http://luohaha.github.io/Chinese-uvbook/source/introduction.html)
* [C++11 uvpp](https://github.com/larroy/uvpp)
* [C++ uvpp](https://github.com/derekdai/uvpp)
* [libuv_tcp](https://github.com/wqvbjhc/libuv_tcp)