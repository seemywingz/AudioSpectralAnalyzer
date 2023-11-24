function fetchInitialSettings() {
    fetch('/sensitivity', { method: 'GET' })
        .then(response => response.text())
        .then(data => {
            var sensitivityValue = parseInt(data, 10);
            sensitivitySlider.value = sensitivityValue; // Update the slider's value
        })
        .catch(error => {
            console.error('Error fetching initial sensitivity:', error);
        });

    fetch('/brightness', { method: 'GET' })
        .then(response => response.text())
        .then(data => {
            var brightnessValue = parseInt(data, 10);
            brightnessSlider.value = brightnessValue; // Update the slider's value
        })
        .catch(error => {
            console.error('Error fetching initial brightness:', error);
        });
}

function fetchBirdSettings() {
    fetch('/birds', { method: 'GET' })
        .then(response => response.text())
        .then(data => {
            var settings = data.split('\n');
            settings.forEach(setting => {
                var parts = setting.split('=');
                var key = parts[0];
                var value = parts[1];
                switch (key) {
                    case 'birdCount':
                        document.getElementById('birdCount').value = value;
                        break;
                    case 'birdAlignment':
                        document.getElementById('birdAlignment').value = value;
                        break;
                    case 'birdCohesion':
                        document.getElementById('birdCohesion').value = value;
                        break;
                    case 'birdSeparation':
                        document.getElementById('birdSeparation').value = value;
                        break;
                    case 'birdVerticalBounds':
                        document.querySelector('#birdVerticalBounds').checked = (value === 'true');
                        break;
                    case 'birdHorizontalBounds':
                        document.querySelector('#birdHorizontalBounds').checked = (value === 'true');
                        break;
                }
            });
        })
        .catch(error => {
            console.error('Error fetching bird settings:', error);
        });
}

window.onload = function () {
    fetchInitialSettings();
    fetchBirdSettings();
}

document.querySelectorAll('.toggle-button').forEach(function (toggle) {
    var contentId = toggle.id;
    // Load the saved state
    toggle.checked = localStorage.getItem(contentId) === 'true';
    // Save state on change
    toggle.addEventListener('click', function () {
        localStorage.setItem(contentId, toggle.checked);
    });
});

var sensitivitySlider = document.getElementById('sensitivityValue');
sensitivitySlider.addEventListener('change', function () {
    var formData = new FormData();
    formData.append('value', sensitivitySlider.value);
    fetch('/sensitivity', { method: 'POST', body: formData })
        .then(response => response.text())
        .then(data => {
            console.log(data);
        })
        .catch(error => {
            console.error('Error setting sensitivity:', error);
        });
});

var brightnessSlider = document.getElementById('brightnessValue');
brightnessSlider.addEventListener('change', function () {
    var formData = new FormData();
    formData.append('value', brightnessSlider.value);
    fetch('/brightness', { method: 'POST', body: formData })
        .then(response => response.text())
        .then(data => {
            console.log(data);
        })
        .catch(error => {
            console.error('Error setting brightness:', error);
        });
});

var visualizationSelect = document.getElementById('visualizationSelect');
visualizationSelect.addEventListener('change', function () {
    var selectedValue = visualizationSelect.value;
    document.getElementById('bird-settings').style.display = 'none';
    document.getElementById('text-settings').style.display = 'none';

    switch (selectedValue) {
        case '2':
            document.getElementById('bird-settings').style.display = 'block';
            setVisualization(selectedValue);
            break;
        case 'TEXT':
            document.getElementById('text-settings').style.display = 'block';
            break;
        default:
            setVisualization(selectedValue);
    }
});

function setVisualization(mode) {
    fetch('/visualization?mode=' + mode, { method: 'GET' })
        .then(response => response.text())
        .then(data => {
            console.log(data);
        })
        .catch(error => {
            console.error('Error setting visualization mode:', error);
        });
}

document.getElementById('sendText').addEventListener('click', function () {
    var text = document.getElementById('customText').value;
    var textColor = document.getElementById('textColor').value;
    var textSpeed = document.getElementById('textSpeed').value;

    fetch('/text', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/x-www-form-urlencoded',
        },
        body: 'text=' + encodeURIComponent(text)
            + '&textColor=' + encodeURIComponent(textColor)
            + '&textSpeed=' + encodeURIComponent(textSpeed)
    })
        .then(response => response.text())
        .then(data => console.log('Text and speed updated:', data))
        .catch(error => console.error('Error:', error));
});

document.getElementById('updateBirds').addEventListener('click', function () {
    var birdCount = document.getElementById('birdCount').value;
    var birdAlignment = document.getElementById('birdAlignment').value;
    var birdCohesion = document.getElementById('birdCohesion').value;
    var birdSeparation = document.getElementById('birdSeparation').value;
    var birdVerticalBounds = document.getElementById('birdVerticalBounds').checked;
    var birdHorizontalBounds = document.getElementById('birdHorizontalBounds').checked;

    fetch('/birds', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/x-www-form-urlencoded',
        },
        body: 'birdCount=' + birdCount
            + '&birdAlignment=' + birdAlignment
            + '&birdCohesion=' + birdCohesion
            + '&separation=' + birdSeparation
            + '&birdVerticalBounds=' + birdVerticalBounds
            + '&birdHorizontalBounds=' + birdHorizontalBounds
    })
        .then(response => response.text())
        .then(data => console.log(data))
        .catch(error => console.error('Error:', error));
});

document.getElementById('wifiSetup').addEventListener('click', function () {
    window.location.href = '/setup';
});

document.getElementById('wifiErase').addEventListener('click', function () {
    window.location.href = '/erase';
});


