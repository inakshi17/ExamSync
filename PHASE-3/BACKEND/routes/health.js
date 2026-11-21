const express = require('express');
const router = express.Router();

// GET /health tells us if the server is alive
router.get('/', (req, res) => {
  res.json({
    status: 'ok',
    uptime: process.uptime(),
    timestamp: new Date().toISOString(),
  });
});

module.exports = router;