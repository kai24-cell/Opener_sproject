#pragma once
#include <string>
#include <iostream>
#include <cstdio>
#include <memory>
#include <array>
#include <cstdlib>
#include "json.hpp" // includeフォルダにあるjson.hppを使用
#include "NoteSummary.hpp"

using json = nlohmann::json;

class NoteService {
private:
    std::string get_env_var(const char* key) {
        char* val = std::getenv(key);
        if (val == nullptr) {
            std::cerr << "警告: 環境変数 " << key << " が設定されていません。" << std::endl;
            return "";
        }
        return std::string(val);
    }

    std::string call_azure_api(const std::string& prompt) {
        std::string key = get_env_var("AZURE_OPENAI_KEY");
        std::string endpoint = get_env_var("AZURE_OPENAI_ENDPOINT");
        std::string deploy_name = get_env_var("AZURE_OPENAI_DEPLOYMENT_NAME");
        std::string api_version = get_env_var("AZURE_OPENAI_API_VERSION");

        if (key.empty() || endpoint.empty()) return "Error: API配置情報が不足しています。";

        // URLの組み立て（エンドポイントの末尾に/があってもなくても大丈夫なように調整してもいいですが、
        // 今は.envが正しいと信じてシンプルに繋ぎます）
        std::string url = endpoint + "/openai/deployments/" + deploy_name + "/chat/completions?api-version=" + api_version;

        // 【修正ポイント】メッセージオブジェクトを明確に作成してから配列に入れる
        json message_obj = {
            {"role", "user"},
            {"content", "以下の内容を要約して。回答は必ず summary, content, conclusion の3キーを持つJSON形式の日本語で。内容：" + prompt}
        };

        json request_body = {
            {"messages", json::array({ message_obj })} 
        };

        // curlコマンドの構築
        // 注意: Windows環境や特殊文字を含む場合、エスケープ処理が必要になることがありますが、
        // まずは基本的な疎通確認のためこのまま行きます。
        std::string command = "curl -X POST \"" + url + "\" " +
                              "-H \"Content-Type: application/json\" " +
                              "-H \"api-key: " + key + "\" " +
                              "-d '" + request_body.dump() + "'";

        std::array<char, 256> buffer;
        std::string response;
        
        // 実行と結果の取得
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
        if (!pipe) {
            return "Error: curlコマンドの実行に失敗しました。";
        }
        
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            response += buffer.data();
        }
        return response;
    }

public:
NoteSummary summarize_note(const std::string& raw_text) {
        std::string ai_response = call_azure_api(raw_text);
        NoteSummary result;

        try {
            auto j = json::parse(ai_response);
            
            // Azure OpenAI のレスポンス構造を掘り下げる
            std::string content_str = j["choices"][0]["message"]["content"];
            
            // AIが生成したJSON文字列をパース
            auto inner_j = json::parse(content_str);
            
            // 1. summary (要約) の取得
            if (inner_j.contains("summary") && inner_j["summary"].is_string()) {
                result.summary = inner_j["summary"].get<std::string>();
            } else {
                result.summary = "要約を取得できませんでした";
            }

            // 2. content (内容) の取得 - ここを強化！
            if (inner_j.contains("content")) {
                if (inner_j["content"].is_array()) {
                    // 配列なら、一つずつ取り出して箇条書きにする
                    std::string joined_text = "";
                    for (const auto& item : inner_j["content"]) {
                        if (item.is_string()) {
                            joined_text += "・" + item.get<std::string>() + "\n";
                        }
                    }
                    result.content = joined_text;
                } else if (inner_j["content"].is_string()) {
                    // 文字列ならそのまま使う
                    result.content = inner_j["content"].get<std::string>();
                } else {
                    result.content = "形式が不明です";
                }
            } else {
                result.content = "内容なし";
            }

            // 3. conclusion (結論) の取得
            if (inner_j.contains("conclusion") && inner_j["conclusion"].is_string()) {
                result.conclusion = inner_j["conclusion"].get<std::string>();
            } else {
                result.conclusion = "結論なし";
            }
            
        } catch (const std::exception& e) {
            result.summary = "解析エラー";
            // 念のため、エラー時は生の応答を表示してデバッグしやすくする
            result.content = "AIの応答形式が想定と異なります。\nエラー: " + std::string(e.what()) + "\n\n生の応答:\n" + ai_response;
            result.conclusion = "再試行してみてください";
        }

        return result;
    }
};