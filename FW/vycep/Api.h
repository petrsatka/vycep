#ifndef Api_h
#define Api_h
#include <Arduino.h>
#include "User.h"
#include "Utils.h"
class Api {
public:
  Api(const User &user);
  ~Api();

/*
createAdmin = function(username, password,
api.createUser = function(username, password
api.changePassword = function(oldPassword, password
api.login = function(username, password,
api.getQueueCount 
api.getCurrentUserBillCount
api.getUserBillCount = function(username,
api.makeOrder
api.pay = function(username, count,
api.loadUsers
api.logout
*/


private:
  const User &user;
};
#endif