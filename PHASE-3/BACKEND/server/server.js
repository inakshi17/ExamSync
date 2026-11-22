const express = require('express');
const path = require('path');
const { router: auth } = require('./auth');

const app = express();
app.use(express.json());
app.use(express.static(path.join(__dirname, '..', 'public')));
app.use('/api', auth);
app.use('/api/admin', require('./admin'));
app.use('/api', require('./user'));
app.use((err, req, res, next) => res.status(500).json({ error: err.message }));

const PORT = process.env.PORT || 5000;
app.listen(PORT, () => console.log(`ExamSync running at http://localhost:${PORT}`));
