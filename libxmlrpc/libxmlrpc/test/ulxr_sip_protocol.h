#ifndef ULXR_SIP_PROTOCOL_H
#define ULXR_SIP_PROTOCOL_H

#include <map>
#include <vector>
#include <string>

using namespace std;

enum State {
	ConnStart,
	ConnPendingCR,
	ConnPendingHeaderLine,
	ConnHeaderLine,
	ConnSwitchToBody,
	ConnBody,
	ConnError
};

class Protocol {
private:
	State state;
	int content_length;
public:
	Protocol();
	virtual bool hasBytesToRead() const = 0;
	virtual void setRemainingContentLength(long len) = 0;
	virtual long getRemainingContentLength() const = 0;
	virtual State connectionMachine(char * &buffer, long &len) = 0;
	virtual int readRaw(char *buffer, int len) = 0;
	void setConnectionState(State stat);
	State getConnectionState();
	void setContentLength(int len);
	int getContentLength();
};

/** Runs sip as protocol for rpc transmition.
  * @ingroup grp_ulxr_protocol
  */
class SipProtocol : public Protocol
{
 public:

   typedef std::map<string, string> header_property;
   SipProtocol();
   virtual ~SipProtocol();
   void clearSipInfo();
   virtual void resetConnection();
   void setRemainingContentLength(long len);
   long getRemainingContentLength() const;
   bool hasBytesToRead() const;
   void parseHeaderLine();
   void makeLower( string &str);
   string stripWS(const string &s);
   void determineContentLength();
   void dumpProperties();
   void buildRequestHeader(const string &user,
								const string &from,
                                const string &to,
                                const string &via,
								const string &call_id,
								const string cseq,
								const string &content
								);
   string getSipProperty (const string &name) const;
   bool hasSipProperty (const string &name);
   virtual int readRaw(char *buffer, int len);
   /*string getFirstHeaderLine() const;
   */
   virtual State connectionMachine(char * &buffer, long &len);
 private:
   void machine_switchToBody(char * &buffer,
                             long &len,
                             char * &chunk_start,
                             char * &chunk_cursor);/**/
 private:
	 string sip_str;
	 int pos;
     struct PImpl;
     PImpl *pimpl;
};

#endif