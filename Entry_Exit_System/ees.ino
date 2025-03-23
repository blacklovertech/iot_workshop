#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>

const char* ssid = "Your_SSID";
const char* password = "Your_PASSWORD";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
String filename = "/data.txt";

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) { delay(1000); Serial.print("."); }
    Serial.println("\nConnected to WiFi!");

    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS Mount Failed!");
        return;
    }

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html", getWebPage());
    });

    server.on("/scan", HTTP_POST, [](AsyncWebServerRequest *request) {
        String barcode = "";
        if (request->hasParam("barcode", true)) {
            barcode = request->getParam("barcode", true)->value();
        }
        String response = handleScan(barcode);
        request->send(200, "text/plain", response);
        ws.textAll(getLogData());
    });

    ws.onEvent([](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
        if (type == WS_EVT_CONNECT) client->text(getLogData());
    });

    server.addHandler(&ws);
    server.begin();
}

void loop() {
    ws.cleanupClients();
}

String handleScan(String barcode) {
    File file = SPIFFS.open(filename, "r");
    String data = "", line;
    bool found = false;

    while (file.available()) {
        line = file.readStringUntil('\n');
        if (line.startsWith(barcode)) {
            found = true;
            if (line.indexOf("OPEN") != -1) {
                data += barcode + "," + getTime() + ",CLOSED\n";
            } else {
                data += barcode + "," + getTime() + ",OPEN\n";
            }
        } else {
            data += line + "\n";
        }
    }
    file.close();

    if (!found) data += barcode + "," + getTime() + ",OPEN\n";

    file = SPIFFS.open(filename, "w");
    file.print(data);
    file.close();

    return found ? "Exit recorded" : "New entry recorded";
}

String getTime() {
    return "2025-03-23 14:00:00";  // Replace with real-time module if needed
}

String getLogData() {
    File file = SPIFFS.open(filename, "r");
    String data = "[";
    while (file.available()) {
        String line = file.readStringUntil('\n');
        if (line.length() > 0) {
            String parts[3];
            int index = 0;
            for (int i = 0; i < 3; i++) {
                index = line.indexOf(",");
                parts[i] = line.substring(0, index);
                line = line.substring(index + 1);
            }
            data += "{\"id\":\"" + parts[0] + "\",\"time\":\"" + parts[1] + "\",\"status\":\"" + parts[2] + "\"},";
        }
    }
    file.close();
    return data.substring(0, data.length() - 1) + "]";
}

String getWebPage() {
    return R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Entry-Exit System</title>
    <script src="https://unpkg.com/html5-qrcode"></script>
    <script>
        let socket = new WebSocket("ws://" + location.host + "/ws");
        socket.onmessage = event => updateTable(JSON.parse(event.data));

        function scanBarcode() {
            const scanner = new Html5QrcodeScanner("reader", { fps: 10, qrbox: 250 });
            scanner.render(result => {
                scanner.clear();
                fetch("/scan", {
                    method: "POST",
                    headers: { "Content-Type": "application/x-www-form-urlencoded" },
                    body: "barcode=" + result
                }).then(response => response.text()).then(data => {
                    document.getElementById("status").innerText = data;
                });
            });
        }

        function updateTable(data) {
            let table = "<tr><th onclick='sortTable(0)'>ID</th><th onclick='sortTable(1)'>Time</th><th onclick='sortTable(2)'>Status</th></tr>";
            data.forEach(row => {
                table += `<tr><td>${row.id}</td><td>${row.time}</td><td>${row.status}</td></tr>`;
            });
            document.getElementById("log").innerHTML = table;
        }

        function sortTable(col) {
            let table = document.getElementById("log");
            let rows = Array.from(table.rows).slice(1);
            let sorted = rows.sort((a, b) => a.cells[col].innerText.localeCompare(b.cells[col].innerText));
            table.append(...sorted);
        }

        function searchTable() {
            let input = document.getElementById("search").value.toLowerCase();
            let rows = document.querySelectorAll("#log tr:not(:first-child)");
            rows.forEach(row => row.style.display = row.innerText.toLowerCase().includes(input) ? "" : "none");
        }
    </script>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; }
        button, input { padding: 10px; margin: 10px; font-size: 16px; }
        table { margin: auto; border-collapse: collapse; width: 90%; }
        th, td { padding: 10px; border: 1px solid black; text-align: left; }
        th { cursor: pointer; background: lightgray; }
        #reader { width: 300px; margin: auto; }
    </style>
</head>
<body>
    <h2>Entry-Exit Log</h2>
    <div id="reader"></div>
    <button onclick="scanBarcode()">Scan Barcode</button>
    <p id="status"></p>
    <input type="text" id="search" onkeyup="searchTable()" placeholder="Search for barcodes..">
    <h3>Log:</h3>
    <table id="log" border="1"></table>
</body>
</html>
    )rawliteral";
}
