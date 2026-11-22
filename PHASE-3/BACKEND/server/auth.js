// Login (checked by the C++ hash-table Auth class) + token sessions + role guard.
const crypto = require('crypto');
const router = require('express').Router();
const { run } = require('./engine');

const sessions = new Map(); // token -> { role, refId }

router.post('/login', async (req, res) => {
  const { username, password } = req.body;
  if (!username || !password) return res.status(400).json({ ok: false });
  const r = await run('login', username, password);
  if (!r.ok) return res.status(401).json({ ok: false, error: 'Invalid username or password' });
  const token = crypto.randomUUID();
  sessions.set(token, { role: r.role, refId: r.refId });
  res.json({ ok: true, token, role: r.role, refId: r.refId });
});

// usage: need('ADMIN') or need('FACULTY','ADMIN')
const need = (...roles) => (req, res, next) => {
  const s = sessions.get(req.headers['x-token']);
  if (!s) return res.status(401).json({ error: 'Please login' });
  if (!roles.includes(s.role)) return res.status(403).json({ error: 'Not allowed' });
  req.user = s;
  next();
};

module.exports = { router, need };
