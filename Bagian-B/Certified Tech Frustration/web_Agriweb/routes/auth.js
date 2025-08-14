import db from '../utils/database.js';
import { generateToken } from '../utils/jwt.js';

// Default profile and settings
const DEFAULT_PROFILE = JSON.stringify({
  favoriteCrop: "wheat",
  experienceLevel: "intermediate",
  farmSize: 50
});

const DEFAULT_SETTINGS = JSON.stringify({
  irrigationMode: "scheduled",
  waterLevel: 26,
  climateControl: "optimal",
  temperature: 22,
  cropRotation: "seasonal",
  humidity: 29,
  fertilizerType: "synthetic",
  lightIntensity: 23
});

export function registerUser(username, email, password) {
  return new Promise((resolve, reject) => {
    if (!username || !email || !password) {
      return reject(new Error('All fields required'));
    }
    
    db.run('INSERT INTO users (username, email, password, profile, settings) VALUES (?, ?, ?, ?, ?)', 
      [username, email, password, DEFAULT_PROFILE, DEFAULT_SETTINGS], function(err) {
      if (err) {
        return reject(new Error('Username or email already exists'));
      }
      
      const user = {
        id: this.lastID,
        username,
        role: 'farmer'
      };

      const token = generateToken(user);
      resolve({ user, token });
    });
  });
}

export function loginUser(username, password) {
  return new Promise((resolve, reject) => {
    db.get('SELECT * FROM users WHERE username = ?', [username], (err, user) => {
      if (err || !user) {
        return reject(new Error('Invalid credentials'));
      }
      
      if (password === user.password) {
        const userData = {
          id: user.id,
          username: user.username,
          role: user.role
        };

        const token = generateToken(userData);
        resolve({ user: userData, token });
      } else {
        reject(new Error('Invalid credentials'));
      }
    });
  });
}

export function getUserById(userId) {
  return new Promise((resolve, reject) => {
    db.get('SELECT * FROM users WHERE id = ?', [userId], (err, user) => {
      if (err || !user) {
        return reject(new Error('User not found'));
      }
      resolve(user);
    });
  });
} 