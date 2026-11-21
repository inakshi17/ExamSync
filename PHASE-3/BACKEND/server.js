const express = require('express');
const dotenv = require('dotenv');
const healthRoutes = require('./routes/health');

dotenv.config();

const app = express();
app.use(express.json());

// Health check: GET /health
app.use('/health', healthRoutes);

const PORT = process.env.PORT || 5000;
app.listen(PORT, () => {
  console.log(`Server running on port ${PORT}`);
});
