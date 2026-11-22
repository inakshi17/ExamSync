// Student and faculty routes (each user only sees their own data).
const router = require('express').Router();
const { run } = require('./engine');
const { need } = require('./auth');

router.get('/my/seat', need('STUDENT'), async (req, res) => res.json(await run('student', req.user.refId)));
router.get('/my/duties', need('FACULTY'), async (req, res) => res.json(await run('duties', req.user.refId, '-')));
router.get('/my/ufm', need('FACULTY'), async (req, res) => res.json(await run('ufm', 'mine', req.user.refId)));
router.post('/my/ufm', need('FACULTY'), async (req, res) => {
  const b = req.body;
  res.json(await run('ufmadd', b.roll, b.subject, b.room, req.user.refId, b.type, b.details));
});

module.exports = router;

