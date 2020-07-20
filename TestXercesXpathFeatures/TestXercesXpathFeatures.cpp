#include "TestXercesXpathFeatures.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
using namespace xercesc;

#include <iostream>
#include <string>
using namespace std;

#define TEST_FILE "sample.xml"

// This program is only for testing XPath evaluation in Xerces
// The code is not clean and has memory leak
int main(int argc, char* argv[])
{
    cout << "This is a program to test Xerces 3.0.0+ XPath 1.0 support.\n"
        << "'sample.xml' next to TestXercesXpathFeatures.exe will be used for testing.\n"
        << "If you want to use your XML file, rename and replace sample.xml.\n"
        << "This program only take 1 argument as XPath. Pass in more than 1 the program will only use the 1st one.\n";

    if (argc == 0)
    {
        cout << "Please pass in a XPath argument" << endl;
        return 1;
    }

    const char* xpath = argv[1];

    XMLPlatformUtils::Initialize();

    // create the DOM parser
    XercesDOMParser* parser = new XercesDOMParser;
    parser->setValidationScheme(XercesDOMParser::Val_Never);
    parser->parse(TEST_FILE);

    // get the DOM representation
    DOMDocument* doc = parser->getDocument();

    // get the root element
    DOMElement* root = doc->getDocumentElement();

    // evaluate the xpath
    DOMXPathResult* result = nullptr;
    try
    {
        result = doc->evaluate(
            XMLString::transcode(xpath),
            root,
            NULL,
            DOMXPathResult::ORDERED_NODE_SNAPSHOT_TYPE,
            NULL
        );
    }
    catch (const DOMXPathException& e)
    {
        cout << "An error occurred: " << endl;
        return 1;
    }

    if (result->getNodeValue() == NULL)
    {
        cout << "There is no result for the provided XPath " << endl;
    }
    else
    {
        cout << TranscodeToStr(result->getNodeValue()->getFirstChild()->getNodeValue(), "ascii").str() << endl;
    }

    XMLPlatformUtils::Terminate();
    return 0;
}
