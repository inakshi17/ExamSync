const isValidEmail = (email) =>
  typeof email === 'string' && /^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(email);

const isNonEmpty = (value) =>
  typeof value === 'string' && value.trim().length > 0;

const isValidDate = (value) => !Number.isNaN(Date.parse(value));

module.exports = { isValidEmail, isNonEmpty, isValidDate };
