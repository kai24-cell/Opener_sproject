#pragma once
#include <string>
#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>

struct NoteSummary {
    std::string summary;
    std::string content;
    std::string conclusion;
};

class NoteService {
private:
    // ここに取得したAPIキーを貼る
    const std::string api_key = "YOUR_API_KEY_HERE";

    // Macのシステムを使って通信する関数
    std::string call_gemini_api(const std::string& prompt) {
        // Gemini APIに送るためのcurlコマンドを組み立てる
        std::string command = "curl https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=" + api_key +
                              " -H 'Content-Type: application/json'" +
                              " -X POST" +
                              " -d '{\"contents\": [{\"parts\":[{\"text\":\"" + prompt + "\"}]}]}'";

        std::array<char, 128> buffer;
        std::string result;
        // コマンドを実行して結果を読み取る
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
        if (!pipe) throw std::runtime_error("popen() failed!");
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        return result;
    }

public:
    NoteService() {
        std::cout << "NoteService: Geminiエンジン接続準備完了！" << std::endl;
    }

    NoteSummary summarize_note(const std::string& raw_text) {
        // AIへの命令（プロンプト）
        std::string prompt = "以下のノートの内容を要約して。回答は必ず JSON形式で、summary, content, conclusionの3つのキーを持つ日本語の文字列にして。余計な解説は不要。ノート内容：" + raw_text;

        std::string ai_response = call_gemini_api(prompt);

        // 本来はここでJSONを解析(Parse)しますが、
        // まずはAIから返ってきた生の文字を「summary」に入れて表示させましょう！
        NoteSummary result;
        result.summary = "Geminiからの応答";
        result.content = ai_response; // ここにAIの生返信が入る
        result.conclusion = "API連携成功！";
        
        return result;
    }
};