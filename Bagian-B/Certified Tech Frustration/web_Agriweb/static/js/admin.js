// Admin-specific functions

export function toggleWaterSystem() {
    const status = document.getElementById('waterStatus');
    const bar = document.getElementById('waterBar');
    const btn = document.getElementById('waterBtn');
    
    if (status.textContent === 'OFFLINE') {
        status.textContent = 'ONLINE';
        status.className = 'status-online';
        bar.className = 'metric-fill metric-online';
        bar.style.width = '100%';
        btn.textContent = 'DISABLE WATER';
        btn.className = 'btn-admin w-full py-2 text-xs rounded';
    } else {
        status.textContent = 'OFFLINE';
        status.className = 'status-offline';
        bar.className = 'metric-fill metric-offline';
        bar.style.width = '0%';
        btn.textContent = 'ENABLE WATER';
        btn.className = 'btn-success w-full py-2 text-xs rounded';
    }
}

export function toggleClimateControl() {
    const status = document.getElementById('climateStatus');
    const bar = document.getElementById('climateBar');
    const btn = document.getElementById('climateBtn');
    
    if (status.textContent === 'OFFLINE') {
        status.textContent = 'ONLINE';
        status.className = 'status-online';
        bar.className = 'metric-fill metric-online';
        bar.style.width = '100%';
        btn.textContent = 'DISABLE CLIMATE';
        btn.className = 'btn-admin w-full py-2 text-xs rounded';
    } else {
        status.textContent = 'OFFLINE';
        status.className = 'status-offline';
        bar.className = 'metric-fill metric-offline';
        bar.style.width = '0%';
        btn.textContent = 'ENABLE CLIMATE';
        btn.className = 'btn-success w-full py-2 text-xs rounded';
    }
}

export function toggleFarmingSystems() {
    const status = document.getElementById('farmingStatus');
    const bar = document.getElementById('farmingBar');
    const btn = document.getElementById('farmingBtn');
    
    if (status.textContent === 'OFFLINE') {
        status.textContent = 'ONLINE';
        status.className = 'status-online';
        bar.className = 'metric-fill metric-online';
        bar.style.width = '100%';
        btn.textContent = 'DISABLE FARMING';
        btn.className = 'btn-admin w-full py-2 text-xs rounded';
    } else {
        status.textContent = 'OFFLINE';
        status.className = 'status-offline';
        bar.className = 'metric-fill metric-offline';
        bar.style.width = '0%';
        btn.textContent = 'ENABLE FARMING';
        btn.className = 'btn-success w-full py-2 text-xs rounded';
    }
}

export function togglePowerGrid() {
    const status = document.getElementById('powerStatus');
    const bar = document.getElementById('powerBar');
    const btn = document.getElementById('powerBtn');
    
    if (status.textContent === 'OFFLINE') {
        status.textContent = 'ONLINE';
        status.className = 'status-online';
        bar.className = 'metric-fill metric-online';
        bar.style.width = '100%';
        btn.textContent = 'DISABLE POWER';
        btn.className = 'btn-admin w-full py-2 text-xs rounded';
    } else {
        status.textContent = 'OFFLINE';
        status.className = 'status-offline';
        bar.className = 'metric-fill metric-offline';
        bar.style.width = '0%';
        btn.textContent = 'ENABLE POWER';
        btn.className = 'btn-success w-full py-2 text-xs rounded';
    }
}

export function enableAllSystems() {
    toggleWaterSystem();
    toggleClimateControl();
    toggleFarmingSystems();
    togglePowerGrid();
}

export function emergencyShutdown() {
    const systems = ['water', 'climate', 'farming', 'power'];
    systems.forEach(system => {
        const status = document.getElementById(`${system}Status`);
        const bar = document.getElementById(`${system}Bar`);
        const btn = document.getElementById(`${system}Btn`);
        
        if (status && status.textContent === 'ONLINE') {
            status.textContent = 'OFFLINE';
            status.className = 'status-offline';
            bar.className = 'metric-fill metric-offline';
            bar.style.width = '0%';
            btn.textContent = `ENABLE ${system.toUpperCase()}`;
            btn.className = 'btn-success w-full py-2 text-xs rounded';
        }
    });
}

export function systemReset() {
    if (confirm('Are you sure you want to reset all systems? This will clear all settings and return to default state.')) {
        emergencyShutdown();
        // Additional reset logic can be added here
    }
} 