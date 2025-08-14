import db from '../utils/database.js';

function deepMerge(target, source) {
  const safeTarget = target || Object.create(null);
  
  for (let key in source) {
    if (['__proto__', 'constructor', 'prototype'].includes(key)) continue;
    
    if (source[key] && typeof source[key] === 'object' && !Array.isArray(source[key])) {
      if (!safeTarget[key]) safeTarget[key] = Object.create(null);
      deepMerge(safeTarget[key], source[key]);
    } else {
      safeTarget[key] = source[key];
    }
  }
  return safeTarget;
}

export function updateProfile(userId, profileData) {
  return new Promise((resolve, reject) => {
    db.get('SELECT profile FROM users WHERE id = ?', [userId], (err, user) => {
      if (err || !user) {
        return reject(new Error('User not found'));
      }
      
      try {
        let currentProfile = JSON.parse(user.profile || '{}');
        const updatedProfile = deepMerge(currentProfile, profileData);
        
        db.run('UPDATE users SET profile = ? WHERE id = ?', 
          [JSON.stringify(updatedProfile), userId], (err) => {
          if (err) {
            return reject(new Error('Failed to update profile'));
          }
          
          resolve(updatedProfile);
        });
      } catch (error) {
        reject(new Error('Invalid profile format'));
      }
    });
  });
}

export function updateSettings(userId, settingsData) {
  return new Promise((resolve, reject) => {
    db.get('SELECT settings FROM users WHERE id = ?', [userId], (err, user) => {
      if (err || !user) {
        return reject(new Error('User not found'));
      }
      
      try {
        let currentSettings = JSON.parse(user.settings || '{}');
        const updatedSettings = { ...currentSettings, ...settingsData };
        
        db.run('UPDATE users SET settings = ? WHERE id = ?', 
          [JSON.stringify(updatedSettings), userId], (err) => {
          if (err) {
            return reject(new Error('Failed to update settings'));
          }
          
          resolve(updatedSettings);
        });
      } catch (error) {
        reject(new Error('Invalid settings format'));
      }
    });
  });
}


export function getUser(userId) {
  return new Promise((resolve, reject) => {
    db.get('SELECT username, role, profile, settings FROM users WHERE id = ?', [userId], (err, user) => {
      if (err || !user) {
        return reject(new Error('User not found'));
      }

      try {
        resolve(user);
      } catch (error) {
        reject(new Error('Failed to get user'));
      }
    });
  });
} 