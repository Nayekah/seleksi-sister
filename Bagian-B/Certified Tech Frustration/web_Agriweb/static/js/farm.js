// Farm configuration and display functions


export function updateFarmDisplay(settings) {
    // Update system status
    if (settings.systemStatus) {
        updateSystemStatus(settings.systemStatus);
    }

    // Update greenhouse status
    if (settings.greenhouses) {
        const greenhouseGrid = document.getElementById('greenhouseGrid');
        if (greenhouseGrid) {
            greenhouseGrid.innerHTML = ''; // Clear existing content
            
            settings.greenhouses.forEach((house, index) => {
                const houseElement = document.createElement('div');
                houseElement.className = `system-card p-4 rounded-xl ${house.status === 'online' ? 'status-online' : 'status-offline'}`;
                houseElement.innerHTML = `
                    <div class="flex items-center justify-between mb-2">
                        <span class="text-xs">Greenhouse ${index + 1}</span>
                        <span class="text-xs">${house.status.toUpperCase()}</span>
                    </div>
                    <div class="text-xs text-slate-400">
                        <div class="flex justify-between mb-1">
                            <span>Temperature:</span>
                            <span>${house.temperature}°C</span>
                        </div>
                        <div class="flex justify-between mb-1">
                            <span>Humidity:</span>
                            <span>${house.humidity}%</span>
                        </div>
                        <div class="flex justify-between">
                            <span>Moisture:</span>
                            <span>${house.moisture}%</span>
                        </div>
                    </div>
                `;
                greenhouseGrid.appendChild(houseElement);
            });
        }
    }

    // Update crop status
    if (settings.crops) {
        const cropStatus = document.getElementById('cropStatus');
        if (cropStatus) {
            const healthyCrops = settings.crops.filter(crop => crop.health > 80).length;
            const totalCrops = settings.crops.length;
            const healthPercentage = Math.round((healthyCrops / totalCrops) * 100);
            
            cropStatus.textContent = `${healthPercentage}%`;
            cropStatus.className = healthPercentage > 80 ? 'status-online' : 
                                 healthPercentage > 50 ? 'status-warning' : 'status-offline';
        }
    }
}