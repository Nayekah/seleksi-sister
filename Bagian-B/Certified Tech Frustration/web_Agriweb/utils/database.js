import sqlite3 from 'sqlite3';
import { generateRandomString } from './random.js';


// Initialize SQLite database for AgriWeb
const db = new sqlite3.Database('./db/agriweb.db');

// Create users table
db.serialize(() => {
  db.run(`CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT UNIQUE NOT NULL,
    email TEXT UNIQUE NOT NULL,
    password TEXT NOT NULL,
    role TEXT DEFAULT 'farmer',
    profile TEXT DEFAULT '{}',
    settings TEXT DEFAULT '{}',
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
  )`);
  
  // Check if admin user already exists before creating
  db.get('SELECT id FROM users WHERE username = ?', ['admin'], (err, row) => {
    if (err) {
      console.error('Error checking for admin user:', err);
      return;
    }
    
    // Only create admin user if it doesn't exist
    if (!row) {
      const adminPassword = generateRandomString();
      console.log('Generated admin password:', adminPassword); // Log the password for initial setup
      db.run(`INSERT INTO users (username, email, password, role) 
              VALUES ('admin', 'admin@agriweb.com', ?, 'admin')`, [adminPassword], (err) => {
        if (err) {
          console.error('Error creating admin user:', err);
        } else {
          console.log('Default admin user created successfully');
        }
      });
    }
  });
});

export default db; 