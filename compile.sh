#!/bin/bash
g++ -std=c++17 -I./crow/include -I/opt/homebrew/include main.cpp -o api_server -lSQLiteCpp -lsqlite3 -L/opt/homebrew/lib -lboost_system -lpthread
if [ $? -eq 0 ]; then
  echo "Kompilasi berhasil!"
  echo "Menjalankan server di http://localhost:18080"
  ./api_server
else
  echo "Gagal mengkompilasi. Silakan cek pesan error di atas."
fi