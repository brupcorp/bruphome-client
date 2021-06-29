const s = require('./settings.json');
const fs = require('fs');

const binData = new TextEncoder().encode(JSON.stringify(s));
const fin = new ArrayBuffer(1024);
new Uint8Array(fin).set(binData);

fs.writeFileSync('settings.bin', Buffer.from(fin));

console.log(fin);