const mongoose = require('mongoose');
const dotenv = require('dotenv');
const fs = require('fs');
const csv = require('csv-parser');
const Student = require('./models/Student');

dotenv.config();

const run = async () => {
  await mongoose.connect(process.env.MONGO_URI);
  console.log('Connected for import');

  const rows = [];
  fs.createReadStream('students.csv')
    .pipe(csv())
    .on('data', (row) => {
      rows.push({
        rollNo: row.Roll_No,
        name: row.Student_Name,
        course: row.Course,
        branch: row.Branch,
        semester: Number(row.Semester)
      });
    })
    .on('end', async () => {
      const result = await Student.insertMany(rows, { ordered: false });
      console.log(`${result.length} students imported`);
      process.exit();
    });
};

run();