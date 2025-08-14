// Dashboard specific functions

let currentUser = null;

export function initializeDashboard() {
    // Add slider value update handlers
    document.getElementById('waterLevel').addEventListener('input', (e) => {
        document.getElementById('waterLevelValue').textContent = e.target.value;
    });
    document.getElementById('temperature').addEventListener('input', (e) => {
        document.getElementById('temperatureValue').textContent = e.target.value;
    });
    document.getElementById('humidity').addEventListener('input', (e) => {
        document.getElementById('humidityValue').textContent = e.target.value;
    });
    document.getElementById('lightIntensity').addEventListener('input', (e) => {
        document.getElementById('lightIntensityValue').textContent = e.target.value;
    });

    loadUserInfo();

    // Settings form submission
    document.getElementById('settingsForm').addEventListener('submit', handleSettingsSubmit);


    // Close toast button
    document.getElementById('toastClose').addEventListener('click', () => {
        const toast = document.getElementById('toast');
        toast.classList.remove('translate-y-0', 'opacity-100');
        toast.classList.add('translate-y-full', 'opacity-0');
    });
}

// Load user information
async function loadUserInfo() {
    try {
        const response = await fetch('/challenge/api/user');
        const data = await response.json();
        
        if (data.success) {
            currentUser = data.user;
            document.getElementById('currentUsername').textContent = data.user.username;
            document.getElementById('currentRole').textContent = data.user.role;
            
            // Load profile data
            if (data.user.profile) {
                const profile = JSON.parse(data.user.profile);
                if (profile.favoriteCrop) document.getElementById('favoriteCrop').value = profile.favoriteCrop;
                if (profile.experienceLevel) document.getElementById('experienceLevel').value = profile.experienceLevel;
                if (profile.farmSize) document.getElementById('farmSize').value = profile.farmSize;
            }
            
            // Load current settings into form
            if (data.user.settings) {
                const settings = JSON.parse(data.user.settings);
                // Irrigation System & Water Level
                if (settings.irrigationMode) document.getElementById('irrigationMode').value = settings.irrigationMode;
                if (settings.waterLevel) {
                    document.getElementById('waterLevel').value = settings.waterLevel;
                    document.getElementById('waterLevelValue').textContent = settings.waterLevel;
                }
                // Climate Control & Temperature
                if (settings.climateControl) document.getElementById('climateControl').value = settings.climateControl;
                if (settings.temperature) {
                    document.getElementById('temperature').value = settings.temperature;
                    document.getElementById('temperatureValue').textContent = settings.temperature;
                }
                // Crop Rotation & Humidity
                if (settings.cropRotation) document.getElementById('cropRotation').value = settings.cropRotation;
                if (settings.humidity) {
                    document.getElementById('humidity').value = settings.humidity;
                    document.getElementById('humidityValue').textContent = settings.humidity;
                }
                // Fertilizer Type & Light Intensity
                if (settings.fertilizerType) document.getElementById('fertilizerType').value = settings.fertilizerType;
                if (settings.lightIntensity) {
                    document.getElementById('lightIntensity').value = settings.lightIntensity;
                    document.getElementById('lightIntensityValue').textContent = settings.lightIntensity;
                }
            }
        }
    } catch (error) {
        logActivity(`[ERROR] Failed to load user profile: ${error.message}`, 'error');
    }
}

export async function handleProfileUpdate(event) {
    event.preventDefault();
    
    const profileData = {
        favoriteCrop: document.getElementById('favoriteCrop').value,
        experienceLevel: document.getElementById('experienceLevel').value,
        farmSize: parseInt(document.getElementById('farmSize').value)
    };

    try {
        const response = await fetch('/challenge/api/profile', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            credentials: 'include',
            body: JSON.stringify(profileData)
        });

        const data = await response.json();
        if (data.success) {
            
            logActivity('[SUCCESS] ✓ Profile updated successfully', 'success');
            logActivity(`[PROFILE] Applied settings: ${JSON.stringify(profileData)}`, 'info');
        } else {
            logActivity(`[ERROR] Profile update failed: ${data.error}`, 'error');
        }
    } catch (error) {
        console.error('Profile update error:', error);
        logActivity(`[SYSTEM ERROR] ${error.message}`, 'error');
    }
}


export function showToast(message, type = 'info') {
    const toast = document.getElementById('toast');
    const toastMessage = document.getElementById('toastMessage');
    const toastClose = document.getElementById('toastClose');

    // First ensure the toast is hidden and reset
    toast.classList.add('hidden');
    toast.classList.remove('translate-y-0', 'opacity-100');
    toast.classList.add('translate-y-full', 'opacity-0');

    // Set message and styling
    toastMessage.textContent = message;
    toast.classList.remove('bg-slate-500', 'text-slate-500');
    toast.classList.add(`bg-${type}-500`, `text-${type}-50`);

    // Force a reflow to ensure the animation plays
    void toast.offsetWidth;

    // Show and animate the toast
    toast.classList.remove('hidden');
    requestAnimationFrame(() => {
        toast.classList.remove('translate-y-full', 'opacity-0');
        toast.classList.add('translate-y-0', 'opacity-100');
    });

    // Auto hide after 3 seconds
    const hideTimeout = setTimeout(() => {
        hideToast();
    }, 3000);

    // Add click handler for close button
    toastClose.onclick = () => {
        clearTimeout(hideTimeout);
        hideToast();
    };

    function hideToast() {
        toast.classList.remove('translate-y-0', 'opacity-100');
        toast.classList.add('translate-y-full', 'opacity-0');
        
        // Add a small delay before hiding the element completely
        setTimeout(() => {
            toast.classList.add('hidden');
        }, 300); // Match this with your CSS transition duration
    }
}

// Log activity to terminal
export function logActivity(message, type = 'info') {
    const activityLog = document.getElementById('activityLog');
    const logEntry = document.createElement('div');
    logEntry.className = `mb-2 text-${type === 'error' ? 'red' : type === 'success' ? 'emerald' : type === 'warning' ? 'yellow' : 'slate'}-400`;
    logEntry.textContent = message;
    activityLog.appendChild(logEntry);
    activityLog.scrollTop = activityLog.scrollHeight;
}

// Clear activity log
export function clearLog() {
    const activityLog = document.getElementById('activityLog');
    activityLog.innerHTML = '<div class="text-slate-500">$ _</div>';
}

// Handle settings form submission
async function handleSettingsSubmit(e) {
    e.preventDefault();
    
    const settings = {
        irrigationMode: document.getElementById('irrigationMode').value,
        waterLevel: parseInt(document.getElementById('waterLevel').value),
        climateControl: document.getElementById('climateControl').value,
        temperature: parseInt(document.getElementById('temperature').value),
        cropRotation: document.getElementById('cropRotation').value,
        humidity: parseInt(document.getElementById('humidity').value),
        fertilizerType: document.getElementById('fertilizerType').value,
        lightIntensity: parseInt(document.getElementById('lightIntensity').value),
    };
    
    logActivity('[CONFIG] Updating farm configuration...', 'info');
    
    try {
        const response = await fetch('/challenge/api/settings', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(settings)
        });
        
        const data = await response.json();
        if (data.success) {
            logActivity('[SUCCESS] ✓ Configuration updated successfully', 'success');
            logActivity(`[CONFIG] Applied settings: ${JSON.stringify(settings)}`, 'info');
        } else {
            logActivity(`[ERROR] Configuration update failed: ${data.error}`, 'error');
        }
    } catch (error) {
        logActivity(`[SYSTEM ERROR] ${error.message}`, 'error');
    }
}
