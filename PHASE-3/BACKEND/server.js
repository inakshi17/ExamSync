const express = require('express');
const dotenv = require('dotenv');
const healthRoutes = require('./routes/health');
const requestLogger = require('./middleware/requestLogger');

dotenv.config();

const app = express();
app.use(express.json());
app.use(requestLogger);

// Health check: GET /health
app.use('/health', healthRoutes);

const PORT = process.env.PORT || 5000;
app.listen(PORT, () => {
  console.log(`Server running on port ${PORT}`);
});
