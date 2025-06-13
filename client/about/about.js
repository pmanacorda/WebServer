window.onload = function(){
    fetch("api/about")
    .then(response => {
        return response.json();
    }).then(data => {
        document.getElementById("text").innerText = data[0].Text;
    })
}


$("#submit-button").on("click", function(event) {
    event.preventDefault();
    
    fetch("/api/logout", {
        method: "POST"
    })
    .then(response => {
        if (response.ok) {
            console.log("Logout successful");
            location.href = "index.html"
        } else {
            console.log("Logout failed");
        }
    })
    .then(data => {
       
    })
    .catch(error => {
        console.error("Error:", error);
    });
});