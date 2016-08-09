const Readable = require('stream').Readable;
const readable = new Readable();


readable.push('hello');
readable.push('world');
readable.push(null);

readable.on('data', data => {
  console.log(data.toString());
});

// 读结束后抛end事件，写结束是抛finish事件
readable.on('end', () => {
  console.log('ONEND');
});

// will throw Error: stream.push() after EOF
// readable.push('c');
