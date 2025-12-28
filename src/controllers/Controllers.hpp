#pragma once
#include "../../include/crow_all.h"

class Controllers {
public:
    void initialize_routes(crow::SimpleApp& app){
        CROW_ROUTE(app, "/api/hello")([](){
            return "Setup is complete.";
        });
    }
};
