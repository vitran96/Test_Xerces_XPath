#include "TestXercesXpathFeatures.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOMLSSerializer.hpp>
#include <xercesc/dom/impl/DOMImplementationImpl.hpp>
#include <xercesc/util/XMLUni.hpp>

using namespace xercesc;

#include <iostream>
#include <string>
using namespace std;

#define TEST_FILE "sample.xml"

class DOMPrintErrorHandler : public DOMErrorHandler
{
public:

    DOMPrintErrorHandler() {};
    ~DOMPrintErrorHandler() {};

    /** @name The error handler interface */
    bool handleError(const DOMError& domError) override;
    void resetErrors() {};

private:
    /* Unimplemented constructors and operators */
    //DOMPrintErrorHandler(const DOMErrorHandler &);
    //void operator=(const DOMErrorHandler &);

};

bool DOMPrintErrorHandler::handleError(const DOMError& domError)
{
    // Display whatever error message passed from the serializer
    if (domError.getSeverity() == DOMError::DOM_SEVERITY_WARNING)
        XERCES_STD_QUALIFIER cerr << "\nWarning Message: ";
    else if (domError.getSeverity() == DOMError::DOM_SEVERITY_ERROR)
        XERCES_STD_QUALIFIER cerr << "\nError Message: ";
    else
        XERCES_STD_QUALIFIER cerr << "\nFatal Message: ";

    char* msg = XMLString::transcode(domError.getMessage());
    XERCES_STD_QUALIFIER cerr << msg << XERCES_STD_QUALIFIER endl;
    XMLString::release(&msg);

    // Instructs the serializer to continue serialization if possible.
    return true;
}

// This program is only for testing XPath evaluation in Xerces
// The code is not clean and has memory leak
int main(int argc, char* argv[])
{
    cout << "This is a program to test Xerces 3.0.0+ XPath 1.0 support.\n"
        << "'sample.xml' next to TestXercesXpathFeatures.exe will be used for testing.\n"
        << "If you want to use your XML file, rename and replace sample.xml.\n"
        << "This program only take 1 argument as XPath. Pass in more than 1 the program will only use the 1st one.\n";

    if (argc == 1)
    {
        cout << "Please pass in a XPath argument" << endl;
        return 1;
    }

    std::string xpath(argv[1]);

    cout << "\n";
    cout << "XPath: " << xpath << endl;

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
            XMLString::transcode(xpath.c_str()),
            root,
            NULL,
            DOMXPathResult::ORDERED_NODE_SNAPSHOT_TYPE,
            NULL
        );
    }
    catch (DOMXPathException e)
    {
        cout << "\n";
        cout << "An error occurred: " << XMLString::transcode(e.getMessage()) << endl;
        XMLPlatformUtils::Terminate();
        return 1;
    }

    if (result->getNodeValue() == NULL)
    {
        cout << "There is no result for the provided XPath!" << endl;
    }
    else
    {
        cout << "Found " << result->getSnapshotLength() << endl;
        //TranscodeToStr transcoded(result->getNodeValue()->getFirstChild()->getNodeValue(),"ascii");
        //cout << transcoded.str() << endl;

        //// DOMImpl
        DOMImplementation* domImpl =
            DOMImplementationRegistry::getDOMImplementation(u"");

        //// DOMLSOutput-----------------------------------------
        DOMLSOutput* theOutPut = domImpl->createLSOutput();
        theOutPut->setEncoding(XMLString::transcode("UTF-8"));
        ////-----------------------------------------------------

        //// DOMLSSerializer-------------------------------------
        DOMLSSerializer* theSerializer = domImpl->createLSSerializer();
        ////-----------------------------------------------------

        //// Error Handler---------------------------------------
        DOMErrorHandler* myErrorHandler = new DOMPrintErrorHandler();
        ////-----------------------------------------------------

        //// Configure-------------------------------------------
        DOMConfiguration* serializerConfig = theSerializer->getDomConfig();
        // Set Error Handler
        serializerConfig->setParameter(XMLUni::fgDOMErrorHandler, myErrorHandler);
        // Set Pretty Print
        if (serializerConfig->canSetParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true))
            serializerConfig->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
        ////-----------------------------------------------------

        //// Format Target---------------------------------------
        XMLFormatTarget* myFormTarget = new StdOutFormatTarget();
        ////-----------------------------------------------------

        ////-----------------------------------------------------
        theOutPut->setByteStream(myFormTarget);

        XMLSize_t nLength = result->getSnapshotLength();
        for (XMLSize_t i = 0; i < nLength; i++)
        {
            result->snapshotItem(i);
            theSerializer->write(
                result->getNodeValue(),
                theOutPut
            );
        }
    }

    XMLPlatformUtils::Terminate();
    return 0;
}
