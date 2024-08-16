document.addEventListener('DOMContentLoaded', () => {
    const statusMessage = document.getElementById('status-message');

    fetch('/status')
        .then(response => {
            if (response.ok) {
                return response.text();
            } else {
                throw new Error('Server is not reachable');
            }
        })
        .then(data => {
            statusMessage.textContent = 'Server is up and running: ' + data;
        })
        .catch(error => {
            statusMessage.textContent = 'Server is down: ' + error.message;
        });
});
