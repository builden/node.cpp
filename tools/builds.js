const async = require('async');
const buildOneProj = require('./build-one-proj');
require('colors');

// 需要预先设置VS工程中OutputFile路径为
//   $(OutDir)$(TargetName)$(TargetExt)
// StaticLibrary Application DynamicLibrary
const projs = [
  // {
  //   targetProj: 'deps/g3log/g3logger.vcxproj',
  //   targetType: 'StaticLibrary',
  // },
  // {
  //   targetProj: 'deps/gtest/gtest.vcxproj',
  //   targetType: 'StaticLibrary',
  // },
  // {
  //   targetProj: 'deps/http-parser/http_parser.vcxproj',
  //   targetType: 'StaticLibrary',
  // },
  // {
  //   targetProj: 'deps/openssl/openssl.vcxproj',
  //   targetType: 'StaticLibrary',
  // },
  // {
  //   targetProj: 'deps/uv/libuv.vcxproj',
  //   targetType: 'StaticLibrary',
  // },
  // {
  //   targetProj: 'deps/zlib/zlibstatic.vcxproj',
  //   targetType: 'StaticLibrary',
  //   targetName: 'zlib',
  // },
  // {
  //   targetProj: 'nodecpp/nodecpp.vcxproj',
  //   targetType: 'StaticLibrary',
  // },
  // {
  //   targetProj: '../../qgutil/qgutil.vcxproj',
  //   targetType: 'StaticLibrary',
  // },
];

async.eachOfLimit(projs, 1, (proj, idx, cb) => {
  buildOneProj(proj, cb);
}, (err) => {
  if (err) console.log(err.red);
});
