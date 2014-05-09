#include "TransportManager.h"
#include<map>
#include<sys/socket.h>
#include<sys/types.h>


using namespace srch2::instantsearch;
using namespace srch2::httpwrapper;

void* startListening(void* arg) {
  RouteMap *const map = (RouteMap*) arg;
  const Node& base =  map->getBase();

  hostent *routeHost = gethostbyname(base.getIpAddress().c_str());
  //  if(routeHost == -1) throw std::exception
  struct sockaddr_in routeAddress;
  int fd;

  memset(&routeAddress, 0, sizeof(routeAddress));
  routeAddress.sin_family = AF_INET;
  memcpy(&routeAddress.sin_addr, routeHost->h_addr, routeHost->h_length);
  routeAddress.sin_port = htons(base.getPortNumber());

  if((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("listening socket failed to init");
    exit(255);
  }

  if(bind(fd, (struct sockaddr*) &routeAddress, sizeof(routeAddress)) < 0) {
    perror("listening socket failed to bind");
    exit(255);
  }

  if(listen(fd, 20) == -1) { 
    perror("listening socket failed start");
    exit(255);
  }

  while(1) {
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(sockaddr_in);
    memset(&addr, 0,sizeof(sockaddr_in));
    int newfd;
    if((newfd = accept(fd, (sockaddr*) &addr, &addrlen)) != -1) {
      map->acceptRoute(newfd, *((sockaddr_in*) &addr));
     }
  }

  return NULL;
}

#include "callback_functions.h"

TransportManager::TransportManager(EventBases& bases, Nodes& map) {
  for(Nodes::iterator dest = map.begin(); dest!= map.end(); ++dest) {
    if(dest->thisIsMe) 
      routeMap.setBase(*dest);
    else
      routeMap.addDestination(*dest);
  }
  
  pthread_create(&listeningThread, NULL, startListening, &routeMap);

  routeMap.initRoutes();
  
  while(!routeMap.isTotallyConnected()) {
	  Logger::console("V0: waiting for other nodes!!");
    sleep(10);
  }
  Logger::console("V0: all the nodes are connected!!");

  for(RouteMap::iterator route = routeMap.begin(); route != routeMap.end(); 
      ++route) {
    for(EventBases::iterator base = bases.begin(); 
        base != bases.end(); ++base) {

      struct bufferevent * bev = bufferevent_socket_new(*base, route->second, BEV_OPT_CLOSE_ON_FREE);
      bufferevent_setcb(bev,cb_recieveMessage , NULL, NULL, this);
      bufferevent_enable(bev, EV_READ|EV_WRITE);
      bufferevent_setwatermark(bev,  EV_READ, sizeof(Message), 0);
    }
  }

  distributedTime = 0;
  synchManagerHandler = NULL;
}

MessageTime_t TransportManager::route(NodeId node, Message *msg, 
    unsigned timeout, CallbackReference callback) {
  Connection fd = routeMap.getConnection(node);
  msg->time = __sync_fetch_and_add(&distributedTime, 1);
  if (timeout) {
	  time_t timeOfTimeout_time = timeout + time(NULL);
	  pendingMessages.addMessage(timeout, msg->time, callback);
  }

#ifdef __MACH__
      int flag = SO_NOSIGPIPE;
#else
      int flag = MSG_NOSIGNAL;
#endif

  unsigned totalMsgSize = msg->bodySize + sizeof(Message);
  do{
	  boost::mutex::scoped_lock lock(socketLock);
	  signed err = send(fd, msg, totalMsgSize, flag);
	  if (err < 0) {
		  if(errno == EAGAIN || errno == EWOULDBLOCK) {
			  // wait and try again ?? buffer messages for write ?
			  perror("Send Failed : ");
			  break;
		  } else {
			  perror("Send Failed : ");
			  break;
		  }
	  }
	  if (err == totalMsgSize) {
		  break;
	  }
  } while(0);
  //TODO: errors?
  return msg->time;
}

MessageTime_t TransportManager::route(int fd, Message *msg) {
  msg->time = __sync_fetch_and_add(&distributedTime, 1);

#ifdef __MACH__
      int flag = SO_NOSIGPIPE;
#else
      int flag = MSG_NOSIGNAL;
#endif

  send(fd, msg, msg->bodySize + sizeof(Message), flag);
  //TODO: errors?
  return msg->time;
}

MessageTime_t& TransportManager::getDistributedTime() {
	return distributedTime;
}

CallBackHandler* TransportManager::getInternalTrampoline() {
	return internalMessageBrokerHandler;
}

pthread_t TransportManager::getListeningThread() const {
	return listeningThread;
}

MessageAllocator * TransportManager::getMessageAllocator() {
	return &messageAllocator;
}

PendingMessages * TransportManager::getMsgs() {
	return &pendingMessages;
}

RouteMap * TransportManager::getRouteMap() {
	return &routeMap;
}

CallBackHandler* TransportManager::getSmHandler() {
	return synchManagerHandler;
}
