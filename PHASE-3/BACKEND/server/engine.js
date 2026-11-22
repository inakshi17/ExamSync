// Runs the C++ DSA engine (engine/examsync) and returns its JSON output.
// Calls are queued one at a time so CSV files are never written by two runs at once.
const { execFile } = require('child_process');
const path = require('path');

const ROOT = path.join(__dirname, '..');
const BIN = path.join(ROOT, 'engine', process.platform === 'win32' ? 'examsync.exe' : 'examsync');
let queue = Promise.resolve();

function run(...args) {
  const job = () => new Promise((resolve, reject) => {
    execFile(BIN, args.map(String), { cwd: ROOT, maxBuffer: 1e8 }, (err, out) => {
      if (err) return reject(err);
      try { resolve(JSON.parse(out)); } catch (e) { reject(new Error('bad engine output')); }
    });
  });
  queue = queue.then(job, job);
  return queue;
}

module.exports = { run, ROOT };
