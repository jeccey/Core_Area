#include <ulxr_sip_protocol.h>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <sys/stat.h>
#include <string>
#include <string.h>
#include <cstring>
#include <iostream>

using namespace std;

struct SipProtocol::PImpl
{
	string    header_firstline;
	string    header_buffer;
	long	  remain_content_length;
   header_property       headerprops;
};

SipProtocol::SipProtocol()
  : pimpl(new PImpl)
{
	sip_str = "";
	pos = 0;
}


SipProtocol::~SipProtocol()
{
    delete pimpl;
    pimpl = 0;
}

void SipProtocol::clearSipInfo()
{
    pimpl->header_firstline = "";
    pimpl->header_buffer = "";
    pimpl->headerprops.clear();
}

void SipProtocol::resetConnection()
{
    clearSipInfo();
}

void SipProtocol::setRemainingContentLength(long len)
{
  pimpl->remain_content_length = len;
}


long SipProtocol::getRemainingContentLength() const
{
  return pimpl->remain_content_length;
}

bool SipProtocol::hasBytesToRead() const
{
  bool b = false;
  
  b = getRemainingContentLength() != 0;

  return b;
}


void SipProtocol::parseHeaderLine()
{
    if (pimpl->header_firstline.length() == 0) //不解析第一行
    {
        pimpl->header_firstline = pimpl->header_buffer;
    }
    else
    {
        string nm, cont;
        std::size_t pos = pimpl->header_buffer.find(':');
        if (pos == string::npos)
        {
            nm = pimpl->header_buffer;
            cont = "";
        }
        else
        {
            nm = pimpl->header_buffer.substr(0, pos);
            cont = pimpl->header_buffer.substr(pos+1);
        }

        makeLower(nm);
        cont = stripWS(cont);
        nm = stripWS(nm);
        pimpl->headerprops.insert(std::make_pair(nm, cont));
    }
    pimpl->header_buffer = "";
}

void SipProtocol::makeLower( string &str)
{
  for (unsigned i= 0; i < str.length(); ++i)
      str[i] = tolower(str[i]);
}

string SipProtocol::stripWS(const string &s)
{
   unsigned start = 0;
   while (start < s.length() && isspace(s[start]))
     ++start;

   unsigned end = s.length();
   while (end > start && isspace(s[end-1]))
     --end;

   return s.substr(start, end-start);
}

State
SipProtocol::connectionMachine(char * &buffer, long &len)
{
    if (len == 0 || buffer == 0)
        return getConnectionState();

    char *chunk_cursor = buffer;
    char *chunk_start = buffer;

    while (len > 0)
    {
        const unsigned state = getConnectionState();
        switch (state)
        {
        case ConnStart:
            setConnectionState(ConnHeaderLine);
            clearSipInfo();
        break;

        case ConnPendingCR:
            if (*buffer == '\n') // CR+LF
            {
                --len;
                ++buffer;
            }

            if (pimpl->header_buffer.length() == 0)
                setConnectionState(ConnSwitchToBody);
            else
                setConnectionState(ConnPendingHeaderLine);
        break;

        case ConnPendingHeaderLine:
            if (pimpl->header_buffer.length() == 0)
                setConnectionState(ConnSwitchToBody);

            else if (*buffer != ' ')  // continuation line of current header field?
            {
                parseHeaderLine();
                setConnectionState(ConnHeaderLine);
            }
            else
              setConnectionState(ConnHeaderLine);
        break;

        case ConnHeaderLine:
//            ULXR_TRACE(ULXR_PCHAR("ConnHeaderLine:"));

            if (*buffer == '\r')
                setConnectionState(ConnPendingCR);

            else if (*buffer == '\n')
            {
                if (pimpl->header_buffer.length() == 0)
                    setConnectionState(ConnSwitchToBody);
                else
                    setConnectionState(ConnPendingHeaderLine);
            }

            else
                pimpl->header_buffer += *buffer;

            ++buffer;
            --len;
        break;

        case ConnSwitchToBody:
          machine_switchToBody(buffer, len, chunk_start, chunk_cursor);
        break;

        case ConnBody:
            return ConnBody;
        /*break; */

        case ConnError:
            return ConnError;
        /*break; */

        default:
            setConnectionState(ConnError);
            return ConnError;
        }
    }

    if (getConnectionState() == ConnSwitchToBody)
      machine_switchToBody(buffer, len, chunk_start, chunk_cursor);

    return getConnectionState();
}

void SipProtocol::dumpProperties(){
	header_property::iterator it;
	for(it = pimpl->headerprops.begin(); it != pimpl->headerprops.end(); it++){
		cout << (*it).first << "=" << (*it).second << endl;
	}
}

void SipProtocol::machine_switchToBody(char * &buffer,
                                        long   &len,
                                        char * &chunk_start,
                                        char * &chunk_cursor)
{
	determineContentLength();
	if (getContentLength() >= 0)
       setRemainingContentLength(getContentLength() - len);
	setConnectionState(ConnBody);
}

void SipProtocol::determineContentLength()
{
    header_property::iterator it;
    if ((it = pimpl->headerprops.find("content-length")) != pimpl->headerprops.end() )
    {
        setContentLength(atoi((*it).second.c_str()));
    }
    else
    {
        return;
    }

    setRemainingContentLength(getContentLength());
}



bool SipProtocol::hasSipProperty(const string &in_name)
{
    string name = in_name;
    makeLower(name);
    bool b = pimpl->headerprops.find(name) != pimpl->headerprops.end();
    
    return b;
}

void
SipProtocol::buildRequestHeader(const string &user,
								const string &from,
                                const string &to,
                                const string &via,
								const string &call_id,
								const string cseq,
								const string &content
								)
{
    char contlen[40];
	sprintf(contlen, "%ld", content.length());
	//string sip_str;
    
	sip_str = "INVITE sip: " + user + " SIP/2.0\r\n";
	sip_str += "Via: SIP/2.0/TCP " + via + "\r\n";
	sip_str += "To: " + to + "\r\n";
	sip_str += "From: " + from + "\r\n";
	sip_str += "Max-Forwards: 70\r\n";
	sip_str += "Call-ID: " + call_id + "\r\n";
	sip_str += "CSeq: " + cseq + "\r\n";
	sip_str += "Content-Type: RVSS/xml\r\n";
	sip_str += "Content-Length: "; 
	sip_str += contlen;
	sip_str += "\r\n\r\n";
	sip_str += content;
}

int SipProtocol::readRaw(char *buffer, int len)
{
	if(!buffer) return 0;

	if(pos >= sip_str.length())
		return 0;
	if(len > sip_str.length() - pos)
		len = sip_str.length() - pos;
	strncpy(buffer, sip_str.c_str() + pos,len);
	pos += len;
	return len;
}

Protocol::Protocol(){
	state = ConnStart;
}
void Protocol::setConnectionState(State stat){
	state = stat;
}
State Protocol::getConnectionState(){
	return state;
}
void Protocol::setContentLength(int len) {
	content_length = len;
}
int Protocol::getContentLength(){
	return content_length;
}