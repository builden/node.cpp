// ²»´íµÄ½éÉÜÎÄÕÂ
// http://blog.rootk.com/post/tutorial-for-http-parser.html

#include "stdafx.h"
#include "http-parse-test.h"

#include <http_parser.h>
#pragma comment(lib, "http_parser")

TEST_F(HttpParseTest, http_parser_parse_url) {
  string url = "http://user:pass@host.com:8080/p/a/t/h?query=string&for=bar#hash";
  http_parser_url u = { 0 };
  if (0 == http_parser_parse_url(url.c_str(), url.length(), 0, &u)) {
    if (u.field_set & (1 << UF_SCHEMA)) {
      string schema = url.substr(u.field_data[UF_SCHEMA].off, u.field_data[UF_SCHEMA].len);
      EXPECT_EQ(schema, "http");
    }

    if (u.field_set & (1 << UF_HOST)) {
      string host = url.substr(u.field_data[UF_HOST].off, u.field_data[UF_HOST].len);
      EXPECT_EQ(host, "host.com");
    }

    if (u.field_set & (1 << UF_PORT)) {
      EXPECT_EQ(u.port, 8080);
    }
    else {
      EXPECT_EQ(u.port, 0);
    }

    if (u.field_set & (1 << UF_PATH)) {
      string path = url.substr(u.field_data[UF_PATH].off, u.field_data[UF_PATH].len);
      EXPECT_EQ(path, "/p/a/t/h");
    }

    if (u.field_set & (1 << UF_QUERY)) {
      string query = url.substr(u.field_data[UF_QUERY].off, u.field_data[UF_QUERY].len);
      EXPECT_EQ(query, "query=string&for=bar");
    }

    if (u.field_set & (1 << UF_FRAGMENT)) {
      string fragment = url.substr(u.field_data[UF_FRAGMENT].off, u.field_data[UF_FRAGMENT].len);
      EXPECT_EQ(fragment, "hash");
    }

    if (u.field_set & (1 << UF_USERINFO)) {
      string userinfo = url.substr(u.field_data[UF_USERINFO].off, u.field_data[UF_USERINFO].len);
      EXPECT_EQ(userinfo, "user:pass");
    }
  }
}

string simpleRequest = R"(GET / HTTP/1.1
Host: ip.qq.com
Connection: keep-alive
Pragma: no-cache
Cache-Control: no-cache
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.106 Safari/537.36
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8
Accept-Encoding: gzip, deflate, sdch
Accept-Language: zh-CN,zh;q=0.8

)";

string simpleResponse = R"(HTTP/1.1 200 OK
Server: tws
Date: Fri, 08 Jul 2016 09:12:00 GMT
Content-Type: text/html
Connection: keep-alive
Keep-Alive: timeout=50
Cache-Control: max-age=0, must-revalidate

<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>Document</title>
</head>
<body>
  
</body>
</html>
)";

TEST_F(HttpParseTest, request) {
  http_parser *parser = (http_parser *)malloc(sizeof(http_parser));
  http_parser_settings settings;

  settings.on_message_begin =
    [](http_parser* parser) -> int {
    cout << "on_message_begin" << endl;
    return 0;
  };

  // called after the url has been parsed.
  settings.on_url =
    [](http_parser* parser, const char* at, size_t len) -> int {
    string url(at, at + len);
    cout << "on_url: " << url << endl;
    return 0;
  };

  settings.on_status =
    [](http_parser* parser, const char* at, size_t len) -> int {
    string status(at, at + len);
    cout << "on_status: " << status << endl;
    return 0;
  };

  // called when there are either fields or values in the request.
  settings.on_header_field =
    [](http_parser* parser, const char* at, size_t len) -> int {
    string header_field(at, at + len);
    cout << "on_header_field: " << header_field << endl;
    return 0;
  };

  // called when header value is given
  settings.on_header_value =
    [](http_parser* parser, const char* at, size_t len) -> int {
    string headerValue(at, at + len);
    cout << "on_header_value: " << headerValue << endl;
    return 0;
  };

  // called once all fields and values have been parsed.
  settings.on_headers_complete =
    [](http_parser* parser) -> int {
    cout << "on_headers_complete" << endl;
    return 0;
  };

  // called when there is a body for the request.
  settings.on_body =
    [](http_parser* parser, const char* at, size_t len) -> int {
    string body(at, at + len);
    cout << "on_body: " << body << endl;
    return 0;
  };

  // called after all other events.
  settings.on_message_complete =
    [](http_parser* parser) -> int {
    cout << "on_message_complete" << endl;
    return 0;
  };

  http_parser_init(parser, HTTP_REQUEST);
  size_t bytes_parsed = http_parser_execute(parser, &settings, simpleRequest.c_str(), simpleRequest.length());
  cout << "parsed bytes: " << bytes_parsed << endl;

  free(parser);
}

TEST_F(HttpParseTest, response) {
  // simpleResponse = s.replaceAll(simpleResponse, "\n", "\r\n");
  http_parser *parser = (http_parser *)malloc(sizeof(http_parser));
  http_parser_settings settings;

  settings.on_message_begin =
    [](http_parser* parser) -> int {
    cout << "on_message_begin" << endl;
    return 0;
  };

  // called after the url has been parsed.
  settings.on_url =
    [](http_parser* parser, const char* at, size_t len) -> int {
    string url(at, at + len);
    cout << "on_url: " << url << endl;
    return 0;
  };

  settings.on_status =
    [](http_parser* parser, const char* at, size_t len) -> int {
    string status(at, at + len);
    cout << "on_status: " << status << endl;
    return 0;
  };

  // called when there are either fields or values in the request.
  settings.on_header_field =
    [](http_parser* parser, const char* at, size_t len) -> int {
    string header_field(at, at + len);
    cout << "on_header_field: " << header_field << endl;
    return 0;
  };

  // called when header value is given
  settings.on_header_value =
    [](http_parser* parser, const char* at, size_t len) -> int {
    string headerValue(at, at + len);
    cout << "on_header_value: " << headerValue << endl;
    return 0;
  };

  // called once all fields and values have been parsed.
  settings.on_headers_complete =
    [](http_parser* parser) -> int {
    cout << "on_headers_complete" << endl;
    return 0;
  };

  // called when there is a body for the request.
  settings.on_body =
    [](http_parser* parser, const char* at, size_t len) -> int {
    string body(at, at + len);
    cout << "on_body: " << body << endl;
    return 0;
  };

  // called after all other events.
  settings.on_message_complete =
    [](http_parser* parser) -> int {
    cout << "on_message_complete" << endl;
    return 0;
  };

  http_parser_init(parser, HTTP_RESPONSE);
  size_t bytes_parsed = http_parser_execute(parser, &settings, simpleResponse.c_str(), simpleResponse.length());
  cout << "parsed bytes: " << bytes_parsed << endl;

  free(parser);
}