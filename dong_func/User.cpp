#include "User.hpp"

//ToDo: 유저에 대한 operator 권한을 어떻게 할 것인지 정해야함.



//-----------------------------new----------------------------
void	User::setUserMsg(std::string msg) {
	this->msg = msg;
}

std::string	User::getUserMsg(void) const {
	return msg;
}

void	User::setUserSock(int sockNum) {
	this->sockNum = sockNum;
}

int		User::getUserSock(void) const {
	return sockNum;
}

void	User::setUserState(int state) {
	this->state = state;
}

int		User::getUserState(void) const {
	return state;
}

//------------------------------------------------------------
User::User(void) {
}

User::~User() {}
