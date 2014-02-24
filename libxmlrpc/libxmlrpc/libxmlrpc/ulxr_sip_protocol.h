/***************************************************************************
               ulxr_sip_prootocol.h  -  sip prootocol
                             -------------------
    begin                : Mon May 3 2004
    copyright            : (C) 2002-2007 by Ewald Arnold
    email                : ulxmlrpcpp@ewald-arnold.de

    $Id: ulxr_sip_protocol.h 1089 2007-11-10 10:40:43Z ewald-arnold $

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

#ifndef ULXR_SIP_PROTOCOL_H
#define ULXR_SIP_PROTOCOL_H

#include <ulxmlrpcpp.h>  // always first header

#include <map>
#include <vector>

#include <ulxr_protocol.h>


namespace ulxr {

class TcpIpConnection;
class ConnectorWrapperBase;

/** Runs sip as protocol for rpc transmition.
  * @ingroup grp_ulxr_protocol
  */
class ULXR_API_DECL0 SipProtocol : public Protocol
{
 public:

   typedef std::map<CppString, CppString> header_property;

 /** Constructs a Protocol.
   * @param  conn      pointer to connection object
   * @param  hostname  name of host
   * @param  hostport  port of host
   */
   SipProtocol(Connection *conn, const CppString &hostname, unsigned hostport);

 /** Constructs a Protocol.
   * @param  conn      pointer to connection object
   */
   SipProtocol(TcpIpConnection *conn);

 /** Destroys the Protocol.
   */
   virtual ~SipProtocol();

 /** Tests if the response was successful regarding the transportation.
   * @param   phrase  return value describing the problem.
   * @return true  response is OK.
   */
   virtual bool responseStatus(CppString &phrase) const;

 /** Sends a MethodCall over the connection.
   * @param   call      pointer to the calling data
   * @param   resource  resource for rpc on remote host
   * @param   wbxml     true: data is sent as wbxml
   */
   virtual void sendRpcCall(const MethodCall &call,
                                           const CppString &resource,
                                           bool wbxml_mode);

 /** Sends a MethodResponse over the connection.
   * @param   resp   pointer to the response data
   * @param   wbxml  true: data is sent as wbxml
   */
   virtual void sendRpcResponse(const MethodResponse &resp, bool wbxml_mode);

 /** Resets the state of the Protocol.
   * Before starting a transfer you should call this to ensure
   * a defined state of the internal state machine processing the
   * protocol.
   */
   virtual void resetConnection();

 /** Process a chunk of input data from the connection.
   * Usually you call read() to get some more data and pass it to
   * this method. It processes the header.
   * Normally you only have to take care of two states: ConnError which
   * results in terminating the connection. ConnBody on the other side
   * indicates user data which might be fed to a parser. @see Dispatcher.
   * @param  buffer   pointer to input data
   * @param len       valid length of buffer
   * @return   current state of the processing. This return state may differ from the
   *           state returned by getConnectionState() due to internal processing.
   *           You should ignore everything except ConnBody and ConnError.
   */
   virtual State connectionMachine(char * &buffer, long &len);

 /** Sets the current transmission to expect/accept no return value.
   */
   virtual void setTransmitOnly();

 /** Tests if the current transmission expects a return value.
   * @return true: return value for request
   */
   virtual bool isTransmitOnly();

  /** Returns the protocol name.
   * @return protocol name
   */
   virtual CppString getProtocolName();

 /** Tests if there are bytes left in the message body.
   * @return true: there is at least one byte available
   */
   virtual bool hasBytesToRead() const;


 /** Gets a property field from the sip header.
   * The names are case insensitive. If the property is not available
   * RuntimeException is throw so you should before with hasSipProperty.
   * @param   name   the name of the field
   * @return  the content of the field.
   */
   
   CppString getSipProperty (const CppString &name) const;

 /** Tests if a property field from the sip header is available.
   * The names are case insensitive.
   * @param   name   the name of the field
   * @return  true if the property field is available
   */
   
   bool hasSipProperty (const CppString &name) const;

 /** Gets the first line from the sip header.
   * the forst line contains special information depending on the
   * direction from or to the server.
   * @return  date as string
   */
   CppString getFirstHeaderLine() const;

 /** Returns a cloned SipProtocol
   * @return pointer to the cloned object
   */
   virtual SipProtocol *clone() const;

 /** Detaches the protocol by creating a duplicate of
   * the protocol + connection and closing the original connection afterwards.
   * @return the current protocol
   */
   virtual Protocol *detach();

 /** Shuts down the socket.
   * @param mode  shutdown mode
   *              @li Unix:  SHUT_RD, SHUT_WR or SHUT_RDWR
   *              @li Win32: SD_RECEIVE, SD_SEND or SD_BOTH
   */
   virtual void shutdown(int mode);

 /** Closes the connection.
   */
   virtual void close();

/** Sends a http request header.
   * The actual data must follow directly afterwards.
   * @param  method     the http method in use
   * @param  resource   the requested resource
   * @param  type       the content-type of the requesting data
   * @param  len        the length of the data in the requesting body
   * @param  wbxml_mode true: suppress some header fields
   */
   void buildRequestHeader(const CppString &user,
								const CppString &source,
                                const CppString &dest,
                                const CppString &via,
								const CppString &call_id,
								const CppString cseq);

   void buildResponseHeader(const CppString &via);

 /** Writes the body to the data stream. The current encoding is honoured.
   * @param data    pointer to the data
   * @param len     length of the data
   */
   void writeBody(const char *data, unsigned long len);

protected:

 /** Reset all information contained in the sip header.
   */
   void clearSipInfo();
   void clearSipHeader();

 /** Adds a sip header field for the next transmission.
   * The user fields are reset after transmission.
   * @param name     sip header field name
   * @param value    value of header field
   */
   void addOneTimeSipField(const CppString &name, const CppString &value);

 /** Determines the length of the message body.
   */
   virtual void determineContentLength();

 /** Parses the first line of the header.
   * The first line contains special information like path to the
   * data, sip version in use or error information.
   */
   void parseHeaderLine();
 
 /** Sends a CONNECT message to the proxy and waits for the response.
 */
   void doConnect();

 private:

 /** Initializes internal variables.
   */
   void init ();

 /** Statemachine switches to body.
   * @param  buffer       pointer to input data
   * @param len           valid length of buffer
   * @param  chunk_start  pointer to start of chunk data
   * @param  chunk_cursor pointer to current chunk data
   */
   void machine_switchToBody(char * &buffer,
                             long &len,
                             char * &chunk_start,
                             char * &chunk_cursor);
 protected:
   SipProtocol(const SipProtocol&);
 private:
   SipProtocol& operator=(const SipProtocol&);  // empty!

 private:

   struct PImpl;
   PImpl *pimpl;
};

};

#endif