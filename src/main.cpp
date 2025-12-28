#include "crow_all.h"
#include "controllers/Controllers.hpp"
int main()//server test finished
{
    crow::SimpleApp app;
    CROW_ROUTE(app, "/static/<string>")
    ([](std::string filename){
        crow::response res;
        res.set_static_file_info("static/" + filename);
        return res;
    });

    Controllers controllers;
    controllers.initialize_routes(app);
    

    CROW_ROUTE(app, "/")
    ([](){
        crow::mustache::context ctx;
        return crow::mustache::load("index.html").render(ctx);
    });

    app.port(8083).multithreaded().run();//8080は他のサービスで使用中だから今回は8083で統一してる
}