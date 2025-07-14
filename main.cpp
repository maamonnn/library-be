#include "crow.h"
#include <SQLiteCpp/SQLiteCpp.h>
#include <iostream>
#include <string>
#include <vector>

void set_cors_headers(crow::response& res) {
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.add_header("Access-Control-Allow-Headers", "Content-Type");
}

int main()
{
    SQLite::Database db("perpustakaan.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    db.exec("CREATE TABLE IF NOT EXISTS buku (id INTEGER PRIMARY KEY, Judul_Buku TEXT NOT NULL, Tahun_Terbit TEXT NOT NULL, Rak TEXT NOT NULL)");

    crow::SimpleApp app;

    CROW_ROUTE(app, "/buku").methods("OPTIONS"_method)
    ([](){
        crow::response res;
        set_cors_headers(res); // Panggil manual
        res.code = 204;
        return res;
    });

    CROW_ROUTE(app, "/buku").methods("POST"_method)
    ([&db](const crow::request& req){
        crow::response res;
        set_cors_headers(res); // Panggil manual

        auto json_body = crow::json::load(req.body);
        if (!json_body || !json_body.has("judul") || !json_body.has("tahun") || !json_body.has("rak")) {
            res.code = 400;
            res.write("JSON tidak valid atau field tidak lengkap.");
            return res;
        }

        try {
            SQLite::Statement query(db, "INSERT INTO buku (Judul_Buku, Tahun_Terbit, Rak) VALUES (?, ?, ?)");
            query.bind(1, json_body["judul"].s());
            query.bind(2, json_body["tahun"].s());
            query.bind(3, json_body["rak"].s());
            query.exec();
            res.code = 201;
            res.write("Buku berhasil ditambahkan.");
        } catch (const std::exception& e) {
            res.code = 500;
            res.write("Internal Server Error");
        }
        return res;
    });

    CROW_ROUTE(app, "/buku").methods("GET"_method)
    ([&db](){
        crow::response res;
        set_cors_headers(res);

        crow::json::wvalue response_body;
        std::vector<crow::json::wvalue> buku_list;
        try {
            SQLite::Statement query(db, "SELECT id, Judul_Buku, Tahun_Terbit, Rak FROM buku");
            while (query.executeStep()) {
                crow::json::wvalue book;
                book["id"] = query.getColumn(0).getInt();
                book["judul"] = query.getColumn(1).getString();
                book["tahun"] = query.getColumn(2).getString();
                book["rak"] = query.getColumn(3).getString();
                buku_list.push_back(std::move(book));
            }
            response_body["buku"] = std::move(buku_list);
            res.code = 200;
            res.write(response_body.dump());
        } catch (const std::exception& e) {
            res.code = 500;
            res.write("Internal Server Error");
        }
        return res;
    });

    app.port(18080).multithreaded().run();
    return 0;
}