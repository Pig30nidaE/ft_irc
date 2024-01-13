#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <sstream>
#include <unistd.h>
#include <set>

#include "NumericReplies.hpp"

#define	NEEDMOREPARMS 2
#define	PASSMISMATCH 3
#define PASSSUCCESS 1
#define MINUS 1
#define PLUS 2

class User;
class ChannelContainer;
class Channel;
class Mode;

class Command {
	private:
		std::string 			 serverPass;

		int						 checkLogin(User user);
		std::vector<std::string> splitComma(std::string msg);
	public:
		Command(void);
		~Command();
		Command(Command &other);
		Command operator=(Command &other);
		Command(std::string serverPass);

		void			 setServerPass(std::string serverPass);

		void		 	 sendMsg(User &user, const std::string msg) const;
		void			 broadcastMsg(User &user, Channel *channel, const std::string msg) const;

		void			 checkLastPassState(User &user) const;
		int 			 checkMode(char c);
		int 			 checkModeChange(Mode &mode, char c);
		std::string 	 modeParsing(User &user, std::string modestring, Mode mode);

		void			 pass(std::vector<std::string> msgs, User &user);
		void			 nick(std::vector<std::string> msgs, ChannelContainer &cc, User &user);
		void			 user(std::vector<std::string> msgs, User &user);
		void			 join(std::vector<std::string> msgs, ChannelContainer &cc, User &user);
		void			 part(std::vector<std::string> msgs, ChannelContainer &cc, User &user);
		void			 kick(std::vector<std::string> msgs, ChannelContainer &cc, User &user);
		void			 ping(std::vector<std::string> msgs, User &user);
		void			 invite(std::string nick, std::string channel, ChannelContainer &cc, User &user);
		void			 topic(std::vector<std::string> msgs, User& user);
		void		 	 mode(std::vector<std::string> &msgs, User& user);
		void 			 privmsg(std::vector<std::string> msgs, User &user, ChannelContainer &cc);
		void 			 quit(std::vector<std::string> msgs, User &user);

		class ErrNotRegistered : public std::invalid_argument {
			public:
				ErrNotRegistered(const std::string& command) : std::invalid_argument(ERR_NOTREGISTERED(command)) {}
		};

		class ErrUnknownCommand : public std::invalid_argument {
			public:
				ErrUnknownCommand(const std::string& nickname, const std::string &command) : std::invalid_argument(ERR_UNKNOWNCOMMAND(nickname, command)) {}
		};

		class ErrPasswdMisMatch : public std::exception {
			public:
				const char* what(void) const throw() {
					return "irc.local 464 * :Password incorrect\r\n";
				};
		};

		class ErrAlreadyRegistered : public std::invalid_argument { 
			public:
				ErrAlreadyRegistered(const std::string &nickname) : std::invalid_argument(ERR_ALREADYREGISTERED(nickname)) {}
		};

		class ErrNickNameInUser : public std::invalid_argument {
			public:
				ErrNickNameInUser(const std::string& nickname) : std::invalid_argument(ERR_NICKNAMEINUSE(nickname)) {}
		};

		class ErrErroneusNickName : public std::invalid_argument {
			public:
				ErrErroneusNickName(const std::string& nickname) : std::invalid_argument(ERR_ERRONEUSNICKNAME(nickname)) {}
		};

		class ErrTooManyChannels : public std::invalid_argument {
			public:
				ErrTooManyChannels(const std::string &channelName, const std::string &nickName) : std::invalid_argument(ERR_TOOMANYCHANNELS(channelName, nickName)) {}
		};

		class ErrChannelIsFull : public std::invalid_argument {
			public:
				ErrChannelIsFull(const std::string &channelName, const std::string nickName) : std::invalid_argument(ERR_CHANNELISFULL(channelName, nickName)) {}
		};

		class ErrBadChannelKey : public std::invalid_argument {
			public:
				ErrBadChannelKey(const std::string &nickname, const std::string &channelName) : std::invalid_argument(ERR_BADCHANNELKEY(nickname, channelName)) {}
		};

		class ErrInviteOnlyChan : public std::invalid_argument {
			public:
				ErrInviteOnlyChan(const std::string &channelName, const std::string &nickName) : std::invalid_argument(ERR_INVITEONLYCHAN(channelName, nickName)) {}
		};

		class ErrBadChanMask :public std::invalid_argument {
			public:
				ErrBadChanMask(const std::string &channelName) : std::invalid_argument(ERR_BADCHANMASK(channelName)) {}
		};

		class ErrNotonChannel : public std::invalid_argument {
			public:
				ErrNotonChannel(const std::string & channel, const std::string nickName) : std::invalid_argument(ERR_NOTONCHANNEL(channel, nickName)) {}
		};

		class RplChannelModeIs : public std::invalid_argument {
			public:
				RplChannelModeIs(const std::string &nickname, const std::string & channel, const std::string &mode) : std::invalid_argument(RPL_CHANNELMODEIS(nickname, channel, mode)) {}
		};

		class ErrUnknownMode : public std::invalid_argument {
			public:
				ErrUnknownMode(const std::string &nickname, const char &flag) : std::invalid_argument(ERR_UNKNOWNMODE(nickname, flag)) {}
		};

		class ErrKeyMissMatch : public std::invalid_argument {
			public:
				ErrKeyMissMatch(const std::string &nickname, const std::string &channel) : std::invalid_argument(ERR_KEYMISSMATCH(nickname, channel)) {}
		};

		class ErrCannotSendToChan : public std::invalid_argument {
			public:
				ErrCannotSendToChan(const std::string & nickname, const std::string & channel) : std::invalid_argument(ERR_CANNOTSENDTOCHAN(nickname, channel)) {}
		};

		class ErrNoSuchChannel : public std::invalid_argument {
			public:
				ErrNoSuchChannel(const std::string &nickname, const std::string &channel) : std::invalid_argument(ERR_NOSUCHCHANNEL(nickname, channel)) {}
		};

		class ErrNoSuchNick : public std::invalid_argument {
			public:
				ErrNoSuchNick(const std::string &nickname, const std::string &target) : std::invalid_argument(ERR_NOSUCHNICK(nickname, target)) {}
		};

		class ErrNeedMoreParams : public std::invalid_argument {
			public:
				ErrNeedMoreParams(const std::string &nickname, const std::string &command) : std::invalid_argument(ERR_NEEDMOREPARAMS(nickname, command)) {}
		};

		class ErrChanOPrivsNeeded : public std::invalid_argument {
			public:
				ErrChanOPrivsNeeded(const std::string& channel) : std::invalid_argument(ERR_CHANOPRIVSNEEDED(channel)) {}
		};

		class ErrInvalidModeParam : public std::invalid_argument {
			public:
				ErrInvalidModeParam(const std::string &nickname, const std::string& channel, const std::string &mode, const std::string &description) : std::invalid_argument(ERR_INVALIDMODEPARAM(nickname, channel, mode, description)) {}
   		};

		class ErrNoTopic : public std::invalid_argument {
			public:
				ErrNoTopic(const std::string& channel) : std::invalid_argument(channel) {}
		};
};

#endif