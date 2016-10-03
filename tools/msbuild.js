const fs = require('fs');
const path = require('path');
let msbuild = null;

const file = path.join(process.env.ProgramFiles, 'MSBuild/14.0/Bin/MSBuild.exe');
if (fs.existsSync(file)) {
  msbuild = file;
}
else {
  throw `can not find MSBuild at: ${file}`;
}


module.exports = msbuild;