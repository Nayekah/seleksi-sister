// Authentication related functions
import { showToast } from './dashboard.js';

export async function handleLogin(event) {
    event.preventDefault();
    const username = document.getElementById('loginUsername').value;
    const password = document.getElementById('loginPassword').value;

    try {
        const response = await fetch('/challenge/api/login', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            credentials: 'include',
            body: JSON.stringify({ username, password })
        });

        const data = await response.json();
        if (data.success) {
            window.location.href = data.user.role === 'admin' ? '/challenge/admin' : '/challenge';
        } else {
            showToast(data.error);
        }
    } catch (error) {
        console.error('Login error:', error);
        showToast('An error occurred during login');
    }
}

export async function handleRegister(event) {
    event.preventDefault();
    const username = document.getElementById('registerUsername').value;
    const email = document.getElementById('registerEmail').value;
    const password = document.getElementById('registerPassword').value;

    try {
        const response = await fetch('/challenge/api/register', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            credentials: 'include',
            body: JSON.stringify({ username, email, password })
        });

        const data = await response.json();
        if (data.success) {
            window.location.href = '/challenge';
        } else {
            showToast(data.error);
        }
    } catch (error) {
        console.error('Registration error:', error);
        showToast('An error occurred during registration');
    }
}

export async function handleLogout() {
    try {
        const response = await fetch('/challenge/api/logout', {
            method: 'POST',
            credentials: 'include'
        });

        const data = await response.json();
        if (data.success) {
            window.location.href = '/login';
        }
    } catch (error) {
        consoshowToastle.error('Logout error:', error);
        showToast('An error occurred during logout');
    }
} 