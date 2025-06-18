#include "BaseController.h"

namespace Controllers{
    void AboutController::Run(Core::HttpRequest& req, Core::HttpResponse& res){
        std::string content;
        std::string contentType;

        if(req.path == "/about.html"){
            content = readFile("client/about/about.html");
            contentType = "text/html";
        }
        else if(req.path == "/about.css"){
            content = readFile("client/about/about.css");
            contentType = "text/css";
        }
        else if(req.path == "/about.js"){
            content = readFile("client/about/about.js");
            contentType = "application/javascript";
        }
        else if(req.path == "/api/about" && req.method == Core::HttpMethod::GET){
            std::unordered_map<std::string, std::string> aboutMe;
            aboutMe["name"] = "Patrick Manacorda";
            aboutMe["birthplace"] = "Vercelli, Italy";
            aboutMe["university"] = "Seattle University";
            aboutMe["degree"] = "Bachelor's in Computer Science";
            aboutMe["marital_status"] = "Married";
            aboutMe["current_location"] = "Tucson, Arizona";
            aboutMe["spouse_occupation"] = "Medical student at University of Arizona College of Medicine";
            aboutMe["hobby_1"] = "Programming";
            aboutMe["hobby_2"] = "Playing chess";
            aboutMe["hobby_3"] = "Exercising";
            aboutMe["favorite_movie_series_1"] = "Star Wars";
            aboutMe["favorite_movie_series_2"] = "Harry Potter";
            aboutMe["favorite_movie_series_3"] = "Lord of the Rings";
            aboutMe["favorite_video_game"] = "Freelancer";
            aboutMe["favorite_game_year"] = "2003";
            aboutMe["favorite_game_publisher"] = "Microsoft";
            aboutMe["favorite_game_genre"] = "Space simulation";
            aboutMe["recent_game_played"] = "Skyrim";
            aboutMe["gaming_frequency"] = "Not much anymore";
            res.statusCode = 200;
            res.body.push_back(std::move(aboutMe));
            res.headers["Content-Type"] = "application/json";
            return;
        }
        else {
            res.statusCode = 404;
            res.text = "File not found";
            res.headers["Content-Type"] = "text/plain";
            return;
        }

        res.statusCode = 200;
        res.text = content;
        res.headers["Content-Type"] = contentType;
    }
}
