const Stream = require('stream');
// 只读
// 实现_read方法
const Readable = Stream.Readable;

// 可写
// 实现_write方法
const Writeable = Stream.Writable;

const fs = require('fs');
// 里面会创建一个Readable对象，并输出到标准输出（可写流）
fs.createReadStream('./typings.json').pipe(process.stdout);
