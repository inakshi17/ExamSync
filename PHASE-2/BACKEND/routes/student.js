const express = require('express');
const Student = require('../models/Student');
const Seating = require('../models/Seating');
const { protect } = require('../middleware/auth');

const router = express.Router();

// Get seat details by roll number
router.get('/seat/:rollNo', protect, async (req, res) => {
  try {
    const seat = await Seating.findOne({ rollNo: req.params.rollNo });
    if (!seat) return res.status(404).json({ message: 'Seat not found' });
    res.json(seat);
  } catch (error) {
    res.status(500).json({ message: error.message });
  }
});

// Get student profile
router.get('/:rollNo', protect, async (req, res) => {
  try {
    const student = await Student.findOne({ rollNo: req.params.rollNo });
    if (!student) return res.status(404).json({ message: 'Student not found' });
    res.json(student);
  } catch (error) {
    res.status(500).json({ message: error.message });
  }
});

module.exports = router;