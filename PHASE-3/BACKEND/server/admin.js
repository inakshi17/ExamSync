// Admin-only routes. Each one just forwards to a C++ engine command.
const fs = require('fs');
const path = require('path');
const express = require('express');
const router = require('express').Router();
const { run, ROOT } = require('./engine');
const { need } = require('./auth');

router.use(need('ADMIN'));
const type = (q) => (['odd', 'even', 'all'].includes(q) ? q : 'odd');
const date = (q) => (/^\d{2}-\d{2}-\d{4}$/.test(q) ? q : '-');

router.get('/dates', async (req, res) => res.json(await run('dates', type(req.query.type))));
router.post('/run', async (req, res) => {
  const d = req.body.date === 'ALL' ? 'ALL' : date(req.body.date);
  res.json(await run('run', type(req.body.type), d));
});
router.post('/reset', async (req, res) => res.json(await run('reset')));
router.get('/rooms', async (req, res) => res.json(await run('rooms', date(req.query.date))));
router.get('/room', async (req, res) => res.json(await run('room', req.query.q || '', date(req.query.date))));
router.get('/student/:roll', async (req, res) => res.json(await run('student', req.params.roll)));
router.get('/faculty/:id', async (req, res) => res.json(await run('faculty', req.params.id)));

router.post('/duty', async (req, res) => res.json(await run('duty', type(req.body.type))));
router.get('/duties', async (req, res) => res.json(await run('duties', req.query.fid || '-', date(req.query.date))));

router.get('/ufm', async (req, res) => res.json(await run('ufm', req.query.pending ? 'pending' : 'all')));
router.post('/ufm/:id', async (req, res) => res.json(await run('ufmset', req.params.id, req.body.status)));

// upload a new csv (students / exams / rooms): replaces the file in data/
const files = { students: ['students.csv', 'Roll_No'], exams: ['exam_schedule.csv', 'Exam_ID'], rooms: ['rooms.csv', 'Room_ID'] };
router.post('/upload/:kind', express.text({ limit: '50mb', type: '*/*' }), (req, res) => {
  const f = files[req.params.kind];
  const text = (req.body || '').replace(/^\uFEFF/, '');
  if (!f) return res.status(400).json({ error: 'Unknown file type' });
  if (!text.replace(/"/g, '').startsWith(f[1])) return res.status(400).json({ error: `Header must start with ${f[1]}` });
  fs.writeFileSync(path.join(ROOT, 'data', f[0]), text);
  res.json({ ok: true, lines: text.split('\n').length - 1 });
});

module.exports = router;
