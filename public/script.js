document.addEventListener('DOMContentLoaded', () => {
    const statusMessage = document.getElementById('heading-message');

    fetch('/status')
        .then(response => {
            if (response.ok) {
                return response.text();
            } else {
                throw new Error('Server is not reachable');
            }
        })
        .then(data => {
            statusMessage.textContent =  data;
        })
        .catch(error => {
            statusMessage.textContent = 'Server is down: ' + error.message;
        });
});
