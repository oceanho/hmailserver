// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../common/Util/TransparentTransmissionBuffer.h"
#include "../common/BO/Message.h"
#include "../common/TCPIP/AnsiStringConnection.h"

namespace HM
{
   class ByteBuffer;
   class MessageRecipient;
  
   class SMTPClientConnection : public AnsiStringConnection
   {
   public:
      SMTPClientConnection(ConnectionSecurity connection_security,
         boost::asio::io_service& io_service, 
         boost::asio::ssl::context& context,
         shared_ptr<Event> disconnected,
         AnsiString remote_hostname,
         bool validate_remote_certificate);
	   virtual ~SMTPClientConnection();

      void OnCouldNotConnect(const AnsiString &sErrorDescription);

      virtual void ParseData(const AnsiString &Request);
      virtual void ParseData(shared_ptr<ByteBuffer> ) {}
      int SetDelivery(shared_ptr<Message> pDelMsg, std::vector<shared_ptr<MessageRecipient> > &vecRecipients);
           
      void SetAuthInfo(const String &sUsername, const String &sPassword);
   protected:

      virtual void OnConnected();
      virtual void OnHandshakeCompleted();
      virtual void OnHandshakeFailed();
      virtual AnsiString GetCommandSeparator() const;

      virtual void SendData_(const String &sData);
      virtual void OnConnectionTimeout();
      virtual void OnExcessiveDataReceived();
      virtual void OnDataSent();
      virtual void OnReadError(int errorCode);

      virtual bool GetValidateRemoteCertificate();
   private:

      void LogReceivedResponse_(const String &response);

      void ProtocolStateHELOEHLO_(const AnsiString &request);
      void ProtocolSendMailFrom_();
      void ProtocolHELOEHLOSent_(const AnsiString &request);
      void ProtocolSTARTTLSSent_(int code);
      void ProtocolMailFromSent_();
      void ProtocolRcptToSent_(int code, const AnsiString &request);
      void ProtocolData_();

      void HandleHandshakeFailure_();
      bool InternalParseData(const AnsiString &Request);
  	   void ReadAndSend_();
	  
      bool IsPositiveCompletion(int iErrorCode);
      bool IsPermanentNegative(int lErrorCode);

      void LogSentCommand_(const String &sData);
      void StartSendFile_(const String &sFilename);

      void SendQUIT_();

      void ProtocolSendUsername_();
      void ProtocolSendPassword_();

      void UpdateAllRecipientsWithError_(int iErrorCode, const AnsiString &sResponse, bool bPreConnectError);
      void UpdateRecipientWithError_(int iErrorCode, const AnsiString &sResponse,shared_ptr<MessageRecipient> pRecipient, bool bPreConnectError);

      shared_ptr<MessageRecipient> GetNextRecipient_();
      void UpdateSuccessfulRecipients_();

      bool GetServerSupportsESMTP_();

      enum ConnectionState
      {
	      HELO = 1,
         HELOSENT = 9,
         AUTHLOGINSENT = 11,
         USERNAMESENT = 12,
         PASSWORDSENT = 13,
         MAILFROMSENT = 3,
         RCPTTOSENT = 5,
         DATAQUESTION = 6,
         DATACOMMANDSENT = 7,
         SENDINGDATA = 13,
         DATASENT = 8,
         QUITSENT = 14,
         STARTTLSSENT = 15
      };

      void SetState_(ConnectionState eCurState);
  
      ConnectionState current_state_;

      shared_ptr<Message> delivery_message_;


      // These are the recipients which will hMailServer should
      // try to deliver to.
      std::vector<shared_ptr<MessageRecipient> > recipients_;

      // The actual recipients are the recipients we've sent RCPT TO
      // for and the remote server has said OK to.
      std::set<shared_ptr<MessageRecipient> > actual_recipients_;

      bool use_smtpauth_;

      String username_;
      String password_;

      unsigned int cur_recipient_;

      bool session_ended_;

      AnsiString last_sent_data_;
      
      File current_file_;   
      TransparentTransmissionBuffer transmission_buffer_;

      AnsiString multi_line_response_buffer_;

      AnsiString remoteServerBanner_;

      bool validate_remote_certificate_;
   };
}
