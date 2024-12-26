# HTTP-Server-in-C++
HTTP 1.1 [RFC 2616] defines the following methods: OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE and CONNECT. The goal of this project is to implement a minimal HTTP 1.1 server supporting and implementing only the GET and HEAD methods. This protocol typically runs on top of TCP on port 80.

The HTTP protocol is a request/response protocol:

A client sends a request to the server in the form of a request method, URI, and protocol version, followed by a MIME-like message containing request modifiers, client information, and possibly body content over a connection with a server.
The server responds with a status line, including the message's protocol version and a success or error code, followed by a MIME-like message containing server information, entity metainformation, and possibly entity-body content.
Where a URI (Uniform Resource Identifier) is either a URL (Uniform Resource Location) or a URN (Uniform Resource Name). Throughout this document the following notation is used: SP is a blank space and CRLF is a carriage return followed by a line feed character. URIs have been known by many names: WWW addresses, Universal Document Identifiers, Universal Resource Identifiers , and finally the combination of Uniform Resource Locators (URL) and Names (URN). As far as HTTP is concerned, Uniform Resource Identifiers are simply formatted strings which identify a resource via a name, location, or any other characteristic.

### Client Request
The general form for an HTTP/1.1 request is:

```
Method SP Request-URI SP HTTP/1.1 CRLF
([general-header line | request-header line | entity-header line] CRLF)*
CRLF
[message body]
```

