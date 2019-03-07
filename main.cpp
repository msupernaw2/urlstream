#include "urlstream.hpp"
#include <iostream>


int main(int argc, char** argv) {

   //lets get historical stock quotes for Apple from yahoo finance.
   
   //open a stream.
    urlstream url("http://ichart.finance.yahoo.com/table.csv?s=aapl&f=nsl1op");

    //read response
    std::string line;
    while (url.good()) {
        std::getline(url, line);
        std::cout << line << std::endl;
    }
    
    //alternatively
    
    //create an instance of urlstream
    urlstream url2;
    
    //open the stream
    url2.open("http://ichart.finance.yahoo.com");
    
    //create an instance of http request
    http_request request;
    
    //set the method
    request.set_method("GET http://ichart.finance.yahoo.com/table.csv?s=aapl&f=nsl1op");
    
    //send the http request
    url2<<request;
    
    //create an instance of http_response
    http_response response;
    
    //Read the whole server response into a http_response.
    //In this case there will be no header info, so just get the full 
    //reponse from the get_data method.
    url2>>response;
    
    //send data to standard out
    std::cout<<response.get_data();
   
}
