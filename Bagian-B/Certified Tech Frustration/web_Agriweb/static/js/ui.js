// UI related functions
import { handleLogin, handleRegister, handleLogout } from './auth.js';
import { handleProfileUpdate } from './dashboard.js';
import { clearLog } from './dashboard.js';
// import { handleSettingsUpdate } from './farm.js';


export function createParticles() {
    const particlesContainer = document.getElementById('particles');
    if (!particlesContainer) return;
    
    const particleCount = 50;

    for (let i = 0; i < particleCount; i++) {
        const particle = document.createElement('div');
        particle.className = 'particle';
        particle.style.left = `${Math.random() * 100}%`;
        particle.style.top = `${Math.random() * 100}%`;
        particle.style.animationDelay = `${Math.random() * 5}s`;
        particlesContainer.appendChild(particle);
    }
}

export function setupEventListeners() {
    // Login form submission
    const loginForm = document.getElementById('loginForm');
    if (loginForm) {
        loginForm.addEventListener('submit', handleLogin);
    }

    // Register form submission
    const registerForm = document.getElementById('registerForm');
    if (registerForm) {
        registerForm.addEventListener('submit', handleRegister);
    }

    // Logout button
    const logoutBtn = document.getElementById('logoutBtn');
    if (logoutBtn) {
        logoutBtn.addEventListener('click', handleLogout);
    }

    // Profile update form
    const profileForm = document.getElementById('profileForm');
    if (profileForm) {
        profileForm.addEventListener('submit', handleProfileUpdate);
    }

    const clearLogBtn = document.getElementById('clearLogBtn');
    if (clearLogBtn) {
        clearLogBtn.addEventListener('click', clearLog);
    }

    // Tab switching functionality
    const loginTab = document.getElementById('loginTab');
    const registerTab = document.getElementById('registerTab');
    if (loginTab && registerTab) {
        loginTab.addEventListener('click', () => {
            document.getElementById('loginForm').classList.remove('hidden');
            document.getElementById('registerForm').classList.add('hidden');
            loginTab.classList.add('btn-primary');
            loginTab.classList.remove('btn-secondary');
            registerTab.classList.add('btn-secondary');
            registerTab.classList.remove('btn-primary');
        });

        registerTab.addEventListener('click', () => {
            document.getElementById('loginForm').classList.add('hidden');
            document.getElementById('registerForm').classList.remove('hidden');
            registerTab.classList.add('btn-primary');
            registerTab.classList.remove('btn-secondary');
            loginTab.classList.add('btn-secondary');
            loginTab.classList.remove('btn-primary');
        });
    }
} 