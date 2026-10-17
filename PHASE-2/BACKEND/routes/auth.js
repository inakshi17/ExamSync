const express = require('express');
const bcrypt = require('bcryptjs');
const jwt = require('jsonwebtoken');
const Student = require('../models/Student');
const Faculty = require('../models/Faculty');

const router = express.Router();

// Student login
router.post('/student/login', async (req, res) => {
  try {
    const { rollNo, password } = req.body;
    const student = await Student.findOne({ rollNo });
    if (!student) return res.status(404).json({ message: 'Student not found' });

    const isMatch = await bcrypt.compare(password, student.password);
    if (!isMatch) return res.status(401).json({ message: 'Invalid credentials' });

    const token = jwt.sign({ id: student._id, role: 'student' }, process.env.JWT_SECRET, { expiresIn: '1d' });
    res.json({ token, student });
  } catch (error) {
    res.status(500).json({ message: error.message });
  }
});

// Faculty login
router.post('/faculty/login', async (req, res) => {
  try {
    const { facultyId, password } = req.body;
    const faculty = await Faculty.findOne({ facultyId });
    if (!faculty) return res.status(404).json({ message: 'Faculty not found' });

    const isMatch = await bcrypt.compare(password, faculty.password);
    if (!isMatch) return res.status(401).json({ message: 'Invalid credentials' });

    const token = jwt.sign({ id: faculty._id, role: 'faculty' }, process.env.JWT_SECRET, { expiresIn: '1d' });
    res.json({ token, faculty });
  } catch (error) {
    res.status(500).json({ message: error.message });
  }
});

module.exports = router;