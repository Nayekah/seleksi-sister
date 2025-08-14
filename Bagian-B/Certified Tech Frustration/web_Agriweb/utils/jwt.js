import jwt from 'jsonwebtoken';
import { generateRandomString } from './random.js';

const JWT_SECRET = generateRandomString(32);
const COOKIE_NAME = 'auth_token';

export function generateToken(user) {
    const payload = {
        id: user.id,
        username: user.username,
        role: user.role
    };

    if (user.role === 'admin') {
        payload.isAdmin = true;
    }

    return jwt.sign(payload, JWT_SECRET, { expiresIn: '24h' });
}

export function verifyToken(token) {
    try {
        return jwt.verify(token, JWT_SECRET);
    } catch (error) {
        throw new Error('Invalid token');
    }
}

export function getTokenFromCookie(req) {
    return req.cookies[COOKIE_NAME];
}

export function setTokenCookie(res, token) {
    res.cookie(COOKIE_NAME, token, {
        httpOnly: true,
        secure: process.env.NODE_ENV === 'production',
        sameSite: 'strict',
        maxAge: 24 * 60 * 60 * 1000 // 24 hours
    });
}

export function clearTokenCookie(res) {
    res.clearCookie(COOKIE_NAME);
} 