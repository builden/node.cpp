const Readable = require('stream').Readable;
const readable = new Readable();

readable.push('a');
readable.push('b');

readable.on('data', data => console.log(data.toString()));
// 读结束后抛end事件，写结束是抛finish事件
readable.on('end', () => console.log('ONREAD'));

readable.push(null);

// will throw Error: stream.push() after EOF
// readable.push('c');
