// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: user.proto
#ifndef GRPC_user_2eproto__INCLUDED
#define GRPC_user_2eproto__INCLUDED

#include "user.pb.h"

#include <functional>
#include <grpcpp/generic/async_generic_service.h>
#include <grpcpp/support/async_stream.h>
#include <grpcpp/support/async_unary_call.h>
#include <grpcpp/support/client_callback.h>
#include <grpcpp/client_context.h>
#include <grpcpp/completion_queue.h>
#include <grpcpp/support/message_allocator.h>
#include <grpcpp/support/method_handler.h>
#include <grpcpp/impl/proto_utils.h>
#include <grpcpp/impl/rpc_method.h>
#include <grpcpp/support/server_callback.h>
#include <grpcpp/impl/server_callback_handlers.h>
#include <grpcpp/server_context.h>
#include <grpcpp/impl/service_type.h>
#include <grpcpp/support/status.h>
#include <grpcpp/support/stub_options.h>
#include <grpcpp/support/sync_stream.h>

namespace user {

// 用户服务
class UserService final {
 public:
  static constexpr char const* service_full_name() {
    return "user.UserService";
  }
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    // 获取用户信息
    virtual ::grpc::Status GetUserInfo(::grpc::ClientContext* context, const ::user::GetUserInfoRequest& request, ::user::GetUserInfoResponse* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::user::GetUserInfoResponse>> AsyncGetUserInfo(::grpc::ClientContext* context, const ::user::GetUserInfoRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::user::GetUserInfoResponse>>(AsyncGetUserInfoRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::user::GetUserInfoResponse>> PrepareAsyncGetUserInfo(::grpc::ClientContext* context, const ::user::GetUserInfoRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::user::GetUserInfoResponse>>(PrepareAsyncGetUserInfoRaw(context, request, cq));
    }
    class async_interface {
     public:
      virtual ~async_interface() {}
      // 获取用户信息
      virtual void GetUserInfo(::grpc::ClientContext* context, const ::user::GetUserInfoRequest* request, ::user::GetUserInfoResponse* response, std::function<void(::grpc::Status)>) = 0;
      virtual void GetUserInfo(::grpc::ClientContext* context, const ::user::GetUserInfoRequest* request, ::user::GetUserInfoResponse* response, ::grpc::ClientUnaryReactor* reactor) = 0;
    };
    typedef class async_interface experimental_async_interface;
    virtual class async_interface* async() { return nullptr; }
    class async_interface* experimental_async() { return async(); }
   private:
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::user::GetUserInfoResponse>* AsyncGetUserInfoRaw(::grpc::ClientContext* context, const ::user::GetUserInfoRequest& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::user::GetUserInfoResponse>* PrepareAsyncGetUserInfoRaw(::grpc::ClientContext* context, const ::user::GetUserInfoRequest& request, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub final : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());
    ::grpc::Status GetUserInfo(::grpc::ClientContext* context, const ::user::GetUserInfoRequest& request, ::user::GetUserInfoResponse* response) override;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::user::GetUserInfoResponse>> AsyncGetUserInfo(::grpc::ClientContext* context, const ::user::GetUserInfoRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::user::GetUserInfoResponse>>(AsyncGetUserInfoRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::user::GetUserInfoResponse>> PrepareAsyncGetUserInfo(::grpc::ClientContext* context, const ::user::GetUserInfoRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::user::GetUserInfoResponse>>(PrepareAsyncGetUserInfoRaw(context, request, cq));
    }
    class async final :
      public StubInterface::async_interface {
     public:
      void GetUserInfo(::grpc::ClientContext* context, const ::user::GetUserInfoRequest* request, ::user::GetUserInfoResponse* response, std::function<void(::grpc::Status)>) override;
      void GetUserInfo(::grpc::ClientContext* context, const ::user::GetUserInfoRequest* request, ::user::GetUserInfoResponse* response, ::grpc::ClientUnaryReactor* reactor) override;
     private:
      friend class Stub;
      explicit async(Stub* stub): stub_(stub) { }
      Stub* stub() { return stub_; }
      Stub* stub_;
    };
    class async* async() override { return &async_stub_; }

   private:
    std::shared_ptr< ::grpc::ChannelInterface> channel_;
    class async async_stub_{this};
    ::grpc::ClientAsyncResponseReader< ::user::GetUserInfoResponse>* AsyncGetUserInfoRaw(::grpc::ClientContext* context, const ::user::GetUserInfoRequest& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::user::GetUserInfoResponse>* PrepareAsyncGetUserInfoRaw(::grpc::ClientContext* context, const ::user::GetUserInfoRequest& request, ::grpc::CompletionQueue* cq) override;
    const ::grpc::internal::RpcMethod rpcmethod_GetUserInfo_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

  class Service : public ::grpc::Service {
   public:
    Service();
    virtual ~Service();
    // 获取用户信息
    virtual ::grpc::Status GetUserInfo(::grpc::ServerContext* context, const ::user::GetUserInfoRequest* request, ::user::GetUserInfoResponse* response);
  };
  template <class BaseClass>
  class WithAsyncMethod_GetUserInfo : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithAsyncMethod_GetUserInfo() {
      ::grpc::Service::MarkMethodAsync(0);
    }
    ~WithAsyncMethod_GetUserInfo() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status GetUserInfo(::grpc::ServerContext* /*context*/, const ::user::GetUserInfoRequest* /*request*/, ::user::GetUserInfoResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestGetUserInfo(::grpc::ServerContext* context, ::user::GetUserInfoRequest* request, ::grpc::ServerAsyncResponseWriter< ::user::GetUserInfoResponse>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  typedef WithAsyncMethod_GetUserInfo<Service > AsyncService;
  template <class BaseClass>
  class WithCallbackMethod_GetUserInfo : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithCallbackMethod_GetUserInfo() {
      ::grpc::Service::MarkMethodCallback(0,
          new ::grpc::internal::CallbackUnaryHandler< ::user::GetUserInfoRequest, ::user::GetUserInfoResponse>(
            [this](
                   ::grpc::CallbackServerContext* context, const ::user::GetUserInfoRequest* request, ::user::GetUserInfoResponse* response) { return this->GetUserInfo(context, request, response); }));}
    void SetMessageAllocatorFor_GetUserInfo(
        ::grpc::MessageAllocator< ::user::GetUserInfoRequest, ::user::GetUserInfoResponse>* allocator) {
      ::grpc::internal::MethodHandler* const handler = ::grpc::Service::GetHandler(0);
      static_cast<::grpc::internal::CallbackUnaryHandler< ::user::GetUserInfoRequest, ::user::GetUserInfoResponse>*>(handler)
              ->SetMessageAllocator(allocator);
    }
    ~WithCallbackMethod_GetUserInfo() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status GetUserInfo(::grpc::ServerContext* /*context*/, const ::user::GetUserInfoRequest* /*request*/, ::user::GetUserInfoResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerUnaryReactor* GetUserInfo(
      ::grpc::CallbackServerContext* /*context*/, const ::user::GetUserInfoRequest* /*request*/, ::user::GetUserInfoResponse* /*response*/)  { return nullptr; }
  };
  typedef WithCallbackMethod_GetUserInfo<Service > CallbackService;
  typedef CallbackService ExperimentalCallbackService;
  template <class BaseClass>
  class WithGenericMethod_GetUserInfo : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithGenericMethod_GetUserInfo() {
      ::grpc::Service::MarkMethodGeneric(0);
    }
    ~WithGenericMethod_GetUserInfo() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status GetUserInfo(::grpc::ServerContext* /*context*/, const ::user::GetUserInfoRequest* /*request*/, ::user::GetUserInfoResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithRawMethod_GetUserInfo : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawMethod_GetUserInfo() {
      ::grpc::Service::MarkMethodRaw(0);
    }
    ~WithRawMethod_GetUserInfo() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status GetUserInfo(::grpc::ServerContext* /*context*/, const ::user::GetUserInfoRequest* /*request*/, ::user::GetUserInfoResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestGetUserInfo(::grpc::ServerContext* context, ::grpc::ByteBuffer* request, ::grpc::ServerAsyncResponseWriter< ::grpc::ByteBuffer>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithRawCallbackMethod_GetUserInfo : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawCallbackMethod_GetUserInfo() {
      ::grpc::Service::MarkMethodRawCallback(0,
          new ::grpc::internal::CallbackUnaryHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
            [this](
                   ::grpc::CallbackServerContext* context, const ::grpc::ByteBuffer* request, ::grpc::ByteBuffer* response) { return this->GetUserInfo(context, request, response); }));
    }
    ~WithRawCallbackMethod_GetUserInfo() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status GetUserInfo(::grpc::ServerContext* /*context*/, const ::user::GetUserInfoRequest* /*request*/, ::user::GetUserInfoResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerUnaryReactor* GetUserInfo(
      ::grpc::CallbackServerContext* /*context*/, const ::grpc::ByteBuffer* /*request*/, ::grpc::ByteBuffer* /*response*/)  { return nullptr; }
  };
  template <class BaseClass>
  class WithStreamedUnaryMethod_GetUserInfo : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithStreamedUnaryMethod_GetUserInfo() {
      ::grpc::Service::MarkMethodStreamed(0,
        new ::grpc::internal::StreamedUnaryHandler<
          ::user::GetUserInfoRequest, ::user::GetUserInfoResponse>(
            [this](::grpc::ServerContext* context,
                   ::grpc::ServerUnaryStreamer<
                     ::user::GetUserInfoRequest, ::user::GetUserInfoResponse>* streamer) {
                       return this->StreamedGetUserInfo(context,
                         streamer);
                  }));
    }
    ~WithStreamedUnaryMethod_GetUserInfo() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status GetUserInfo(::grpc::ServerContext* /*context*/, const ::user::GetUserInfoRequest* /*request*/, ::user::GetUserInfoResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with streamed unary
    virtual ::grpc::Status StreamedGetUserInfo(::grpc::ServerContext* context, ::grpc::ServerUnaryStreamer< ::user::GetUserInfoRequest,::user::GetUserInfoResponse>* server_unary_streamer) = 0;
  };
  typedef WithStreamedUnaryMethod_GetUserInfo<Service > StreamedUnaryService;
  typedef Service SplitStreamedService;
  typedef WithStreamedUnaryMethod_GetUserInfo<Service > StreamedService;
};

}  // namespace user


#endif  // GRPC_user_2eproto__INCLUDED
