#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <iostream>
#include <string>
#include <string.h>

using namespace std;
using namespace xercesc;

int main(int argc, char *argv[]) {
	string content = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?> <request command=\"curegister\"><parameters>eee</parameters></request>";
	string nodeName;
	try {
		XMLPlatformUtils::Initialize();
	} catch (const XMLException &toCatch) {
		char *message = XMLString::transcode(toCatch.getMessage());
		cout << message << endl;
		XMLString::release(&message);
		return 1;
	}

	{char *memstr = new char[content.length() + 1];
	strcpy(memstr, content.c_str());

	MemBufInputSource mem((XMLByte *)memstr, content.length(), (const XMLCh *)0);
	XercesDOMParser *pDomParser = new XercesDOMParser;
	pDomParser->parse(mem);
	xercesc::DOMDocument *doc = pDomParser->getDocument();
	

	DOMElement *pRoot = doc->getDocumentElement();
	DOMNodeIterator *iter = doc->createNodeIterator(pRoot, DOMNodeFilter::SHOW_ALL, NULL, true);
	
	for(DOMNode *current = iter->nextNode(); current != 0; current = iter->nextNode()) {
		nodeName = XMLString::transcode(current->getNodeName());
		cout << nodeName << endl;
		if(nodeName == "#text")
		cout << XMLString::transcode(current->getNodeValue()) << endl;
	}
	//XMLPlatformUtils::Terminate();

#if 0
	DOMNodeList *nodeList = doc->getElementsByTagName(XMLString::transcode("request"));
	DOMNode *childNode;
	int listLength = nodeList->getLength();
	for(int i = 0; i < listLength; i++) {
		cout << XMLString::transcode(nodeList->item(i)->getNodeName()) << endl;
		if(nodeList->item(i)->hasAttributes()) {
			DOMNamedNodeMap *nodeMap = nodeList->item(i)->getAttributes();
			int length = nodeMap->getLength();
			for(int j = 0 ; j < length; j++){
				char *nodeName = new char[30];
				bool trans = XMLString::transcode(nodeMap->item(j)->getNodeName(), nodeName, 30);
				cout << trans << " " << nodeName << endl;
				//cout << XMLString::transcode(nodeMap->item(i)->getNodeName()) <<endl;
				//cout << XMLString::transcode(nodeMap->item(i)->getNodeValue()) <<endl;
			}
		}
		//childNode = nodeList->item(i)->getFirstChild();
		//cout << XMLString::transcode(childNode->getNodeName()) << endl;

	}
#endif
	}
	/*delete nodeList;
	delete childNode;
	delete doc;
	delete pDomParser;
	*/
	XMLPlatformUtils::Terminate();
	return 0;
}