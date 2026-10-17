const express = require('express');
const UFM = require('../models/UFM');
const { protect, authorizeRoles } = require('../middleware/auth');

const router = express.Router();

// Report a UFM case (faculty only)
router.post('/ufm', protect, authorizeRoles('faculty'), async (req, res) => {
  try {
    const { examId, rollNo, description } = req.body;
    const ufm = await UFM.create({
      examId,
      rollNo,
      facultyId: req.user.id,
      description
    });
    res.status(201).json(ufm);
  } catch (error) {
    res.status(500).json({ message: error.message });
  }
});

module.exports = router;