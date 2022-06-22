#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Util/ServerApplication.h"

#include <cstdint>
#include <iostream>

class HelloRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
    void handleRequest(
        Poco::Net::HTTPServerRequest& request,
        Poco::Net::HTTPServerResponse& response) override
    {
        response.setContentType("text/plain");

        std::ostream& ostr = response.send();
        ostr << "Hello from POCO!";
    }
};

class AppRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
    Poco::Net::HTTPRequestHandler*
        createRequestHandler(const Poco::Net::HTTPServerRequest& request) override
    {
        if(request.getMethod() == "GET" && request.getURI() == "/")
            return new HelloRequestHandler;
        else
            return nullptr;
    }
};

class ServerApp : public Poco::Util::ServerApplication
{
protected:
    virtual int main(const std::vector<std::string>& /*args*/) override
    {
        auto params = new Poco::Net::HTTPServerParams;
        const std::uint16_t port = 5000;
        Poco::Net::HTTPServer srv(new AppRequestHandlerFactory, port);
        srv.start();
        logger().information("Server listening %d", (int)port);
        waitForTerminationRequest();
        srv.stop();
        return Poco::Util::Application::EXIT_OK;
    }
};

POCO_SERVER_MAIN(ServerApp)
