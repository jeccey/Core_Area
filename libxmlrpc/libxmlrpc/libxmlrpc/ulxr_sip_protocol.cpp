/***************************************************************************
               ulxr_sip_protocol.cpp  -  sip prootocol
                             -------------------
    begin                : Mon May 3 2004
    copyright            : (C) 2002-2007 by Ewald Arnold
    email                : ulxmlrpcpp@ewald-arnold.de

    $Id: ulxr_sip_protocol.cpp 1164 2010-01-06 10:03:51Z ewald-arnold $

 ***************************************************************************/

/**************************************************************************
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 ***************************************************************************/

#define ULXR_NEED_EXPORTS
#include <ulxmlrpcpp.h> // always first

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <sys/stat.h>

#include <cstring>

#if defined(__BORLANDC__) || defined (_MSC_VER)
#include <utility>
#endif

#include <ulxr_sip_protocol.h>
#include <ulxr_tcpip_connection.h>
#include <ulxr_except.h>
#include <ulxr_response.h>
#include <ulxr_call.h>

#ifndef ULXR_OMIT_REENTRANT_PROTECTOR
#include <ulxr_mutex.h>
#endif


#include <iostream>
using namespace std;

namespace ulxr {

struct SipProtocol::PImpl
{
   CppString    header_firstline;
   CppString    header_buffer;
   CppString	sip_str;
   CppString    hostname;
   unsigned     hostport;

   ConnectorWrapperBase  *connector;

   header_property       headerprops;
};

ULXR_API_IMPL0 SipProtocol::SipProtocol(const SipProtocol &prot)
  : Protocol(prot)
  , pimpl(new PImpl)
{
  *pimpl = *prot.pimpl;
}

ULXR_API_IMPL0
  SipProtocol::SipProtocol(Connection *conn, const CppString &hn, unsigned hp)
  : Protocol (conn)
  , pimpl(new PImpl)
{
    pimpl->hostname = hn;
    pimpl->hostport = hp;
    ULXR_TRACE(ULXR_PCHAR("SipProtocol(conn, name, port)"));
    init();
}

ULXR_API_IMPL0
  SipProtocol::SipProtocol(TcpIpConnection *conn)
  : Protocol (conn)
  , pimpl(new PImpl)
{
    pimpl->hostname = conn->getPeerName();
    pimpl->hostport = conn->getPort();
    ULXR_TRACE(ULXR_PCHAR("SipProtocol(conn)"));
    init();
}
 
ULXR_API_IMPL0 SipProtocol::~SipProtocol()
{
    ULXR_TRACE(ULXR_PCHAR("~SipProtocol"));
    delete pimpl->connector;
    delete pimpl;
    pimpl = 0;
}


ULXR_API_IMPL(SipProtocol *) SipProtocol::clone() const
{
    ULXR_TRACE(ULXR_PCHAR("SipProtocol::clone()"));
    return new SipProtocol(*this);
}

ULXR_API_IMPL(void) SipProtocol::doConnect()
{
    //empty
}

ULXR_API_IMPL(Protocol *) SipProtocol::detach()
{
    ULXR_TRACE(ULXR_PCHAR("SipProtocol::detach()"));
    SipProtocol *cloneprot = this->clone();
    cloneprot->setConnection(getConnection()->detach());
    cloneprot->pimpl->connector = new ConnectorWrapper<SipProtocol>(cloneprot, &SipProtocol::doConnect);
    return cloneprot;  // return previous and running connection
}

ULXR_API_IMPL(void) SipProtocol::init()
{
    ULXR_TRACE(ULXR_PCHAR("init"));
    pimpl->connector = new ConnectorWrapper<SipProtocol>(this, &SipProtocol::doConnect);
    getConnection()->setConnector(pimpl->connector);
    ULXR_TRACE(ULXR_PCHAR("init"));
    pimpl->headerprops.clear();
	pimpl->sip_str = "";
}

ULXR_API_IMPL(void) SipProtocol::clearSipInfo()
{
    ULXR_TRACE(ULXR_PCHAR("clearSipInfo"));
    pimpl->header_firstline = ULXR_PCHAR("");
    pimpl->header_buffer = ULXR_PCHAR("");
    //pimpl->headerprops.clear();
	//pimpl->sip_str.clear();
}

ULXR_API_IMPL(void) SipProtocol::clearSipHeader()
{
    ULXR_TRACE(ULXR_PCHAR("clearSipHeader"));
    pimpl->headerprops.clear();
	pimpl->sip_str.clear();
}

ULXR_API_IMPL(void) SipProtocol::resetConnection()
{
    ULXR_TRACE(ULXR_PCHAR("resetConnection"));
    Protocol::resetConnection();
    clearSipInfo();
}

ULXR_API_IMPL(void) SipProtocol::close()
{
    ULXR_TRACE(ULXR_PCHAR("close"));
    Protocol::close();
}

ULXR_API_IMPL(void) SipProtocol::shutdown(int mode)
{
    ULXR_TRACE(ULXR_PCHAR("shutdown"));
    if (getConnection() != 0)
      getConnection()->shutdown(mode);
}

ULXR_API_IMPL(void) SipProtocol::parseHeaderLine()
{
    ULXR_TRACE(ULXR_PCHAR("parseHeaderLine"));

    if (pimpl->header_firstline.length() == 0)
    {
        pimpl->header_firstline = pimpl->header_buffer;
        ULXR_DOUT_HTTP(ULXR_PCHAR("firstline: <") << pimpl->header_firstline << ULXR_PCHAR(">"));
    }
    else
    {
        CppString nm, cont;
        std::size_t pos = pimpl->header_buffer.find(':');
        if (pos == CppString::npos)
        {
            nm = pimpl->header_buffer;
            cont = ULXR_PCHAR("");
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

        ULXR_DOUT_HTTP(ULXR_PCHAR("headerprop: <") << nm
                       << ULXR_PCHAR("> + <") << cont << ULXR_PCHAR("> "));
    }
    pimpl->header_buffer = ULXR_PCHAR("");
}

ULXR_API_IMPL(Protocol::State)
SipProtocol::connectionMachine(char * &buffer, long &len)
{
    /*
      Each invokation of this state machine tries to parse one single
      http header line of the buffer. If the content of the buffer is too small
      (no linefeed found) the content is cached in an internal string
      and used the next time.
      buffer points to the beginning of the next line at return if
      a linefeed has been found. In the message body nothing is done.
    */
    ULXR_TRACE(ULXR_PCHAR("connectionMachine with ") << len << ULXR_PCHAR(" bytes"));
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
            ULXR_TRACE(ULXR_PCHAR("ConnBody:"));
            return ConnBody;
        /*break; */

        case ConnError:
            ULXR_TRACE(ULXR_PCHAR("ConnError:"));
            return ConnError;
        /*break; */

        default:
            setConnectionState(ConnError);
            throw ConnectionException(SystemError, ulxr_i18n(ULXR_PCHAR("connectionMachine(): unknown state")), 500);
        }
    }

    if (getConnectionState() == ConnSwitchToBody)
      machine_switchToBody(buffer, len, chunk_start, chunk_cursor);

    ULXR_TRACE(ULXR_PCHAR("/connectionMachine"));

    return getConnectionState();
}

void SipProtocol::machine_switchToBody(char * &buffer,
                                        long   &len,
                                        char * &chunk_start,
                                        char * &chunk_cursor)
{
  ULXR_TRACE(ULXR_PCHAR("ConnSwitchToBody:"));
  if (hasSipProperty(ULXR_PCHAR("content-length")))
  {
	  determineContentLength();

      ULXR_TRACE(ULXR_PCHAR("content_length: ") << getContentLength());
      ULXR_TRACE(ULXR_PCHAR("len: ") << len);
	  if (getContentLength() >= 0)
            setRemainingContentLength(getContentLength() - len);
  }
  setConnectionState(ConnBody);
}

ULXR_API_IMPL(bool) SipProtocol::hasSipProperty(const CppString &in_name) const
{
    CppString name = in_name;
    makeLower(name);
    bool b = pimpl->headerprops.find(name) != pimpl->headerprops.end();
    ULXR_TRACE(ULXR_PCHAR("hasSipProperty: ") << in_name << ULXR_PCHAR(" ") << b);
    return b;
}

ULXR_API_IMPL(CppString) SipProtocol::getSipProperty (const CppString &in_name) const
{
    ULXR_TRACE(ULXR_PCHAR("getHttpProperty ") << in_name);
    CppString name = in_name;
    makeLower(name);
    header_property::const_iterator it;

    if ((it = pimpl->headerprops.find(name)) == pimpl->headerprops.end() )
        throw ConnectionException(NotConformingError,
                                  ulxr_i18n(ULXR_PCHAR("Http property field not available: "))+name, 400);

    return (*it).second;
}

ULXR_API_IMPL(bool) SipProtocol::isTransmitOnly()
{
    return true;
}

ULXR_API_IMPL(void) SipProtocol::setTransmitOnly()
{
    
}

ULXR_API_IMPL(CppString) SipProtocol::getProtocolName()
{
    return ULXR_PCHAR("sip");
}

ULXR_API_IMPL(CppString) SipProtocol::getFirstHeaderLine() const
{
  return pimpl->header_firstline;
}

ULXR_API_IMPL(bool) SipProtocol::responseStatus(CppString &phrase) const
{
    ULXR_TRACE(ULXR_PCHAR("responseStatus"));

    CppString s = stripWS(getFirstHeaderLine());
    if (s.length() == 0)
    {
        s = ulxr_i18n(ULXR_PCHAR("No connection status available"));
        return false;
    }

    std::size_t pos = s.find(' ');
    if (pos != CppString::npos)  // skip version
        s.erase(0, pos+1);
    else
        s = ULXR_PCHAR("");

    CppString stat;
    s = stripWS(s);
    pos = s.find(ULXR_CHAR(' '));
    if (pos != CppString::npos)
    {
        stat = s.substr(0, pos);
        s.erase(0, pos+1);
    }
    else
    {
        stat = s;
        s = ULXR_PCHAR("");
    }

    phrase = stripWS(s);

    return stat == ULXR_PCHAR("200");
}

ULXR_API_IMPL(bool) SipProtocol::hasBytesToRead() const
{
  bool b = false;
  
  b = getRemainingContentLength() != 0;

  ULXR_TRACE(ULXR_PCHAR("hasBytesToRead: " << b));
  return b;
}

ULXR_API_IMPL(void) SipProtocol::determineContentLength()
{
    ULXR_TRACE(ULXR_PCHAR("determineContentLength"));

    header_property::iterator it;
    if ((it = pimpl->headerprops.find(ULXR_PCHAR("content-length"))) != pimpl->headerprops.end() )
    {
        ULXR_TRACE(ULXR_PCHAR(" Content-length: ") << (*it).second);
        setContentLength(ulxr_atoi(getLatin1((*it).second).c_str()));
        ULXR_TRACE(ULXR_PCHAR(" length: ") << getContentLength());
    }
    else
    {
        throw ConnectionException(NotConformingError,
                                  ulxr_i18n(ULXR_PCHAR("Content-Length of message not available")), 411);
    }

    setRemainingContentLength(getContentLength());
    ULXR_TRACE(ULXR_PCHAR(" content_length: ") << getContentLength());
}

ULXR_API_IMPL(void)
SipProtocol::buildRequestHeader(const CppString &user,
								const CppString &from,
                                const CppString &to,
                                const CppString &via,
								const CppString &call_id,
								const CppString cseq)
{
    ULXR_TRACE(ULXR_PCHAR("buildRequestHeader"));
    //char contlen[40];
    //ulxr_sprintf(contlen, "%ld", len );
    
	pimpl->sip_str = ULXR_PCHAR("INVITE sip: ") + user + ULXR_PCHAR(" SIP/2.0\r\n");
	pimpl->sip_str += ULXR_PCHAR("Via: SIP/2.0/TCP ") + via + ULXR_PCHAR("\r\n");
	pimpl->sip_str += ULXR_PCHAR("To: ") + to + ULXR_PCHAR("\r\n");
	pimpl->sip_str += ULXR_PCHAR("From: ") + from + ULXR_PCHAR("\r\n");
	pimpl->sip_str += ULXR_PCHAR("Max-Forwards: 70\r\n");
	pimpl->sip_str += ULXR_PCHAR("Call-ID: ") + call_id + ULXR_PCHAR("\r\n");
	pimpl->sip_str += ULXR_PCHAR("CSeq: ") + cseq + ULXR_PCHAR("\r\n");
	pimpl->sip_str += ULXR_PCHAR("Content-Type: RVSS/xml\r\n");
	pimpl->sip_str += ULXR_PCHAR("Content-Length: "); 
	//pimpl->sip_str += contlen;
	//pimpl->sip_str += ULXR_PCHAR("\r\n");
	//pimpl->sip_str += ULXR_PCHAR("\r\n");
}

ULXR_API_IMPL(void)
SipProtocol::buildResponseHeader(const CppString &via)
{
    ULXR_TRACE(ULXR_PCHAR("buildResponseHeader"));
    
	pimpl->sip_str = ULXR_PCHAR("SIP/2.0 200 OK\r\n");
	pimpl->sip_str += ULXR_PCHAR("Via: SIP/2.0/TCP ") + via + ULXR_PCHAR("\r\n");
	pimpl->sip_str += ULXR_PCHAR("To: ") + getSipProperty("To") + ULXR_PCHAR("\r\n");
	pimpl->sip_str += ULXR_PCHAR("From: ") + getSipProperty("From") + ULXR_PCHAR("\r\n");
	pimpl->sip_str += ULXR_PCHAR("Max-Forwards: 70\r\n");
	pimpl->sip_str += ULXR_PCHAR("Call-ID: ") + getSipProperty("Call-ID") + ULXR_PCHAR("\r\n");
	pimpl->sip_str += ULXR_PCHAR("CSeq: ") + getSipProperty("CSeq") + ULXR_PCHAR("\r\n");
	pimpl->sip_str += ULXR_PCHAR("Content-Type: RVSS/xml\r\n");
	pimpl->sip_str += ULXR_PCHAR("Content-Length: "); 
}

void SipProtocol::sendRpcCall(const MethodCall &call,
                                           const CppString &resource,
                                           bool wbxml_mode) {
ULXR_TRACE(ULXR_PCHAR("sendRpcCall"));
    //resetConnection();

    CppString xml = call.getXml(0)+ULXR_PCHAR("\n");
    ULXR_DOUT_XML(xml);
	
	char contlen[40];
    ulxr_sprintf(contlen, "%ld", xml.length());

	pimpl->sip_str += contlen;
	pimpl->sip_str += ULXR_PCHAR("\r\n");
	pimpl->sip_str += ULXR_PCHAR("\r\n");
    
	CppString msg = pimpl->sip_str + xml; 
	writeRaw(msg.data(), msg.length());
	clearSipHeader();
}


ULXR_API_IMPL(void) SipProtocol::sendRpcResponse(const MethodResponse &resp, bool wbxml_mode)
{
    ULXR_TRACE(ULXR_PCHAR("sendRpcResponse"));

    // doConnect(); must already be pimpl->connected

   
        CppString xml = resp.getXml(0)+ULXR_PCHAR("\n");
        ULXR_DOUT_XML(xml);

        char contlen[40];
    ulxr_sprintf(contlen, "%ld", xml.length());

	pimpl->sip_str += contlen;
	pimpl->sip_str += ULXR_PCHAR("\r\n");
	pimpl->sip_str += ULXR_PCHAR("\r\n");
    
	CppString msg = pimpl->sip_str + xml; 
	writeRaw(msg.data(), msg.length());
	clearSipHeader();
}

}