#ifndef HTML_CONTENT_H
#define HTML_CONTENT_H

#define HTML_CONTENT "<html><head>" \
  "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\">" \
  "<style>" \
  "body {" \
  "font-family: 'Courier New', Courier, monospace;" \
  "margin: 0;" \
  "padding: 20px;" \
  "display: flex;" \
  "justify-content: center;" \
  "align-items: center;" \
  "height: 100vh;" \
  "background-color: #000000;" \
  "color: #00F9A2;" \
  "flex-direction: column;" \
  "text-shadow: 0 0 5px #00F9A2;" \
  "overflow: hidden;}" \
  ".console {" \
  "background: #000000;" \
  "padding: 20px;" \
  "border: 2px solid #00F9A2;" \
  "border-radius: 15px;" \
  "width: 100%;" \
  "max-width: 600px;" \
  "box-sizing: border-box;\"} .console-text {" \
  "margin: 10px 0;" \
  "text-align: center;" \
  "}" \
  "input[type=\"text\"]," \
  "input[type=\"password\"] {" \
  "width: 100%;" \
  "padding: 10px;" \
  "margin: 10px 0;" \
  "border: 1px solid #00F9A2;" \
  "background: #000000;" \
  "color: #00F9A2;" \
  "border-radius: 5px;" \
  "box-sizing: border-box;" \
  "font-size: 16px;}" \
  "input[type=\"submit\"]{" \
  "width: 100%;" \
  "padding: 10px;" \
  "border: 1px solid #00F9A2;" \
  "border-radius: 5px;" \
  "background-color: #000000;" \
  "color: #00F9A2;" \
  "font-size: 16px;" \
  "cursor: pointer;" \
  "box-sizing: border-box;}" \
  "input[type=\"submit\"]{" \
  "background-color: #00F9A2;" \
  "color: #000000;}" \
  "</style>" \
  "</head>" \
  "<body>" \
  "<div class=\"console-text\"><b>D1</b>: Knowledgator 1 device for visualizing machine loading...</div>" \
  "<div class=\"console\">" \
  "<div class=\"console-text\" style=\"font-weight: bold;\">WiFi configuration</div>" \
  "<div class=\"console-text\">Please enter the SSID:</div>" \
  "<form action=\"/save\" method=\"POST\">" \
  "<input type=\"text\" name=\"ssid\" placeholder=\"SSID\">" \
  "<div class=\"console-text\">Please enter the Password:</div>" \
  "<input type=\"password\" name=\"password\" placeholder=\"Password\">" \
  "<input type=\"submit\" value=\"Save\">" \
  "</form>" \
  "<div class=\"console-text\">Knowledgator Engineering LTD 2024</div>" \
  "</div>" \
  "</body>" \
  "</html>" \

#endif