// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../Util/Event.h"

#include "SocketConstants.h"
#include "IOOperationQueue.h"
using boost::asio::ip::tcp;

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> ssl_socket;

namespace HM
{
   class ByteBuffer;
   class SecurityRange;

   class TCPConnection :
      public boost::enable_shared_from_this<TCPConnection>
   {
   public:
      TCPConnection(ConnectionSecurity connection_security,
                    boost::asio::io_service& io_service,    
                    boost::asio::ssl::context& context,
                    shared_ptr<Event> disconnected,
                    AnsiString expected_remote_hostname);
      ~TCPConnection(void);

      enum ShutdownOption
      {
         ShutdownSend,
         ShutdownReceive,
      };

      enum Consts
      {
         BufferSize = 60000
      };

      int GetBufferSize() {return BufferSize; }
      bool Connect(const AnsiString &remote_ip_address, long remotePort, const IPAddress &localAddress);
      
      void Start();
      void SetReceiveBinary(bool binary);

      void PostWrite(const AnsiString &sData);
      void PostWrite(shared_ptr<ByteBuffer> pByteBuffer);
      void PostRead(const AnsiString &delimitor);

      void PostShutdown(ShutdownOption what);
      void PostDisconnect();
      void PostTimeout();
      
      IPAddress GetRemoteEndpointAddress();
      unsigned long GetRemoteEndpointPort();
      unsigned long GetLocalEndpointPort();

      void UpdateLogoutTimer();
      void CancelLogoutTimer();

      void SetSecurityRange(shared_ptr<SecurityRange> securityRange);
      shared_ptr<SecurityRange> GetSecurityRange();

      int GetSessionID();

      bool ReportReadErrors(bool newValue);
      
      boost::asio::ip::tcp::socket& GetSocket() {return socket_;}

      ConnectionSecurity GetConnectionSecurity() {return connection_security_; }

      bool IsSSLConnection(){return is_ssl_;}

   protected:

      void SetTimeout(int seconds);
      AnsiString GetIPAddressString();

      virtual void OnCouldNotConnect(const AnsiString &sErrorDescription) {};
      virtual void OnConnected() = 0;
      virtual void OnHandshakeCompleted() = 0;
      virtual void OnHandshakeFailed() = 0;
      virtual void OnConnectionTimeout() = 0;
      virtual void OnExcessiveDataReceived() = 0;
      virtual void OnDataSent() {};
      virtual void OnReadError(int errorCode) {};

      /* PARSING METHODS */
      virtual void ParseData(const AnsiString &sAnsiString) = 0;
      virtual void ParseData(shared_ptr<ByteBuffer> pByteBuffer) = 0;

      void Handshake();
      
      virtual bool GetValidateRemoteCertificate() = 0;
   private:
      
      void HandleHandshakeFailed_(const boost::system::error_code& error);
      void StartAsyncConnect_(const String &ip_adress, int port);

      static void OnTimeout(boost::weak_ptr<TCPConnection> connection, boost::system::error_code const& err);

      String SafeGetIPAddress();

      bool IsClient();

      void ProcessOperationQueue_();

      void Disconnect();
      void Shutdown(boost::asio::socket_base::shutdown_type, bool removeFromQueue);
      void Write(shared_ptr<ByteBuffer> buffer);
      void Read(const AnsiString &delimitor);

      void HandleConnect(const boost::system::error_code& err);
      void HandleHandshake(const boost::system::error_code& error);
      void HandleRead(const boost::system::error_code& /*error*/,  size_t bytes_transferred);
      void HandleWrite(const boost::system::error_code& /*error*/,  size_t bytes_transferred);

      void ReportDebugMessage(const String &message, const boost::system::error_code &error);
      void ReportError(ErrorManager::eSeverity sev, int code, const String &context, const String &message, const boost::system::system_error &error);
      void ReportError(ErrorManager::eSeverity sev, int code, const String &context, const String &message);
      
      boost::asio::ip::tcp::socket socket_;
      ssl_socket ssl_socket_;

      boost::asio::ip::tcp::resolver resolver_;
      boost::asio::deadline_timer timer_;
      boost::asio::streambuf receive_buffer_;
      boost::asio::ssl::context& context_;

      IOOperationQueue operation_queue_;

      bool receive_binary_;
      ConnectionSecurity connection_security_;
      long remote_port_;
      bool hastimeout__;
      String remote__ip__address_;

      shared_ptr<SecurityRange> security_range_;

      int session_id_;
      int timeout_;

      AnsiString expected_remote_hostname_;
      shared_ptr<Event> disconnected_;
      bool is_ssl_;
      bool is_client_;
   };

}