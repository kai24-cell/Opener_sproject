#pragma once
#include "../../include/crow_all.h"
#include "../service/NoteService.hpp" // 作ったServiceを読み込む

class Controllers {
private:
    NoteService noteService; // Serviceをインスタンス化

public:
    // コンストラクタ
    Controllers() : noteService() {}

    void initialize_routes(crow::SimpleApp& app){
        // 元からある挨拶
        CROW_ROUTE(app, "/api/hello")([](){
            return "Setup is complete.";
        });

        // 機能1：ノート要約の受付窓口
        CROW_ROUTE(app, "/api/summarize").methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req){
            // フロントから送られてきたテキストを取得
            auto x = crow::json::load(req.body);
            if (!x) return crow::response(400);

            std::string text = x["text"].s();
            
            // Serviceに要約を頼む
            NoteSummary res = noteService.summarize_note(text);

            // 結果をJSONで返す
            crow::json::wvalue result;
            result["summary"] = res.summary;
            result["content"] = res.content;
            result["conclusion"] = res.conclusion;
            
            return crow::response(result);
        });
    }
};