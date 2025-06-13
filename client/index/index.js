$("#submit-button").on("click", function(event) {
    event.preventDefault();

    const username = $("#username").val();
    const password = $("#password").val();
    
    fetch("/api/login", {
        method: "POST",
        headers: {
            "Content-Type": "application/json"
        },
        body: JSON.stringify({
            Username: username,
            Password: password
        })
    })
    .then(response => {
        if (response.ok) {
            console.log("Login successful");
            location.href = 'about.html';
        } else {
            console.log("Login failed");
        }
    })
    .catch(error => {
        console.error("Error:", error);
    });
});