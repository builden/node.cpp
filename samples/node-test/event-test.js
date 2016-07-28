const fs = require('fs');
fs.readFile('jsconfig.json', (err, data) => {
  console.log(data.toString());
});

// setInterval(() => {
//   console.log('call at interval');
// }, 1000);