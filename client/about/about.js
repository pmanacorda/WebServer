window.onload = function(){
    fetch("api/about")
    .then(response => {
        return response.json();
    }).then(data => {
        document.getElementById("name").innerText = data[0].name;
        document.getElementById("birthplace").innerText = data[0].birthplace;
        document.getElementById("university").innerText = data[0].university;
        document.getElementById("degree").innerText = data[0].degree;
        document.getElementById("marital_status").innerText = data[0].marital_status;
        document.getElementById("current_location").innerText = data[0].current_location;
        document.getElementById("spouse_occupation").innerText = data[0].spouse_occupation;
        document.getElementById("hobby_1").innerText = data[0].hobby_1;
        document.getElementById("hobby_2").innerText = data[0].hobby_2;
        document.getElementById("hobby_3").innerText = data[0].hobby_3;
        document.getElementById("favorite_movie_series_1").innerText = data[0].favorite_movie_series_1;
        document.getElementById("favorite_movie_series_2").innerText = data[0].favorite_movie_series_2;
        document.getElementById("favorite_movie_series_3").innerText = data[0].favorite_movie_series_3;
        document.getElementById("favorite_video_game").innerText = data[0].favorite_video_game;
        document.getElementById("favorite_game_year").innerText = data[0].favorite_game_year;
        document.getElementById("favorite_game_publisher").innerText = data[0].favorite_game_publisher;
        document.getElementById("favorite_game_genre").innerText = data[0].favorite_game_genre;
        document.getElementById("recent_game_played").innerText = data[0].recent_game_played;
        document.getElementById("gaming_frequency").innerText = data[0].gaming_frequency;
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