#include <crow.h>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <sys/stat.h>
#include <unistd.h>

std::vector<std::string> logs;

void add_log(const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %X") << " | " << message;
    logs.push_back(ss.str());
}

int main() {
    // Проверка прав root
    if (getuid() != 0) {
        std::cerr << "Error: Service must be run as root!" << std::endl;
        return 1;
    }

    crow::SimpleApp app;

    // Создание директории tmp
    mkdir("tmp", 0755);

    CROW_ROUTE(app, "/info")
    .methods("GET"_method, "POST"_method)
    ([](const crow::request& req){
        if (req.method == "POST"_method) {
            // Обработка загрузки файла
            std::string filename = "file_" + std::to_string(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()
                ).count()
            );
            
            std::ofstream file("tmp/" + filename);
            file << req.body;
            file.close();
            
            add_log("File uploaded: " + filename);
            return crow::response(200, "File uploaded");
        }
        else {
            add_log("GET /info");
            return crow::response(200, "NO_ERRORS");
        }
    });

    CROW_ROUTE(app, "/log")
    ([]{
        add_log("GET /log");
        std::string result;
        for (const auto& log : logs) {
            result += log + "\n";
        }
        return crow::response(200, result);
    });

    add_log("Service started");
    app.port(1616).multithreaded().run();
}