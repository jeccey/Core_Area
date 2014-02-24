#include <ulxr_sip_protocol.h>
#include <iostream>
#include <ulxmlrpcpp.h>
#include <unordered_map>

using namespace std;
using namespace ulxr;

void waitForCall(Protocol *protocol)
{

  char buffer[5];
  char *buff_ptr;
  string super_data;

  bool done = false;
  long readed;
  while (!done && ((readed = protocol->readRaw(buffer, sizeof(buffer))) > 0) )
  {
    buff_ptr = buffer;
	//super_data.append(buffer,readed);
    while (readed > 0)
    {
      State state = protocol->connectionMachine(buff_ptr, readed);
      if (state == ConnError){
		  cout << "ConnError" << endl;
		  return;
	  }

      else if (state == ConnSwitchToBody)
      {
		  cout << "ConnSwitchToBody" << endl;
        if (!protocol->hasBytesToRead())
        {
          done = true;
        }
      }
      else if (state == ConnBody)
      {
		  //cout << "ConnBody" << endl;
        super_data.append(buff_ptr,readed);
		protocol->setRemainingContentLength(protocol->getRemainingContentLength() - readed);
        readed = 0;
      }
    }

    if (!protocol->hasBytesToRead())
//        || parser->isComplete())
      done = true;
  }
  cout << super_data << endl;
}

template <class T> struct DisableCompare {
  bool operator() (const T& x, const T& y) const {if(x > y || x < = y) return true;}
  typedef T first_argument_type;
  typedef T second_argument_type;
  typedef bool result_type;
};

template <class T> struct _less {
  bool operator() (const T& x, const T& y) const {return x<y;}
  typedef T first_argument_type;
  typedef T second_argument_type;
  typedef bool result_type;
};

typedef std::unordered_map<CppString, int>  Members;

int main() {
#if 0
	SipProtocol protocol;
	protocol.buildRequestHeader("110@192.168.80.125", //user
								"192.168.80.111",	//from
								"192.168.80.125",	//to
								"192.168.80.125",	//via
								"1"	,				//cseq
								"qega444afa4g6aga4g"//call-id
								);
	waitForCall(&protocol);
	protocol.dumpProperties();
#endif
	DisableCompare<CppString> discom;
	Members val;

	val.insert(pair<CppString, int>("key1", 1));
	val.insert(pair<CppString, int>("key2", 2));

	Members::iterator iter;
	for(iter = val.begin(); iter != val.end(); iter++) {
		cout << iter->first << endl;
	}
}