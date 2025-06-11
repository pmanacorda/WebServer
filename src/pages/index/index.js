console.log('Hello from index.js!');

document.addEventListener('DOMContentLoaded', function() {
    const heading = document.querySelector('h1');

    heading.addEventListener('click', function() {
        heading.style.color = heading.style.color === 'red' ? '#333' : 'red';
    });
});