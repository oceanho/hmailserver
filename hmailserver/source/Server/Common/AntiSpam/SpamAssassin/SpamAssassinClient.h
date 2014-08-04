// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../../TCPIP/AnsiStringConnection.h"

namespace HM
{
   class File;

   class SpamAssassinClient : public AnsiStringConnection
   {
   public:
      SpamAssassinClient(const String &sFile, 
         boost::asio::io_service& io_service, 
         boost::asio::ssl::context& context,
         shared_ptr<Event> disconnected,
         bool &testCompleted);
      ~SpamAssassinClient(void);

      virtual void ParseData(const AnsiString &Request);
      virtual void ParseData(shared_ptr<ByteBuffer> pBuf);

      

      
   protected:

      virtual void OnCouldNotConnect(const AnsiString &sErrorDescription);
      virtual void OnReadError(int errorCode);
      virtual void OnConnected();
      virtual void OnHandshakeCompleted() {};
      virtual void OnHandshakeFailed() {};
      virtual AnsiString GetCommandSeparator() const;
      virtual void OnConnectionTimeout();
      virtual void OnExcessiveDataReceived();

      virtual bool GetValidateRemoteCertificate() {return false;}
   private:

      void FinishTesting_();
      int ParseFirstBuffer_(shared_ptr<ByteBuffer> pBuffer) const;
      bool SendFileContents_(const String &sFilename);

      String command_buffer_;

      String message_file_;
	   int spam_dsize_;
	   int message_size_;
      shared_ptr<File> result_;
      bool &test_completed_;

      int total_result_bytes_written_;
  };
}