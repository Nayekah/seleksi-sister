// Initialize Lucide icons
lucide.createIcons();


import { createParticles, setupEventListeners } from './ui.js';

// Initialize when DOM is loaded
document.addEventListener('DOMContentLoaded', () => {
    createParticles();
    setupEventListeners();
});
