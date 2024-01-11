#ifndef NUMERICREPLIES_HPP
#define NUMERICREPLIES_HPP

#define JOINCHANNEL 4
#define NEWCHANNEL 5
#define RPL_WELCOME_NUM 1
#define RPL_YOURHOST 2
#define RPL_CREATED 3
#define RPL_MYINFO 4
#define RPL_ISUPPORT 5
#define RPL_BOUNCE 10
#define RPL_STATSCOMMANDS 212
#define RPL_ENDOFSTATS 219
#define RPL_UMODEIS 221
#define RPL_STATSUPTIME 242
#define RPL_LUSERCLIENT 251
#define RPL_LUSEROP 252
#define RPL_LUSERUNKNOWN 253
#define RPL_LUSERCHANNELS 254
#define RPL_LUSERME 255
#define RPL_ADMINME 256
#define RPL_ADMINLOC1 257
#define RPL_ADMINLOC2 258
#define RPL_ADMINEMAIL 259
#define RPL_TRYAGAIN 263
#define RPL_LOCALUSERS 265
#define RPL_GLOBALUSERS 266
#define RPL_WHOISCERTFP 276
#define RPL_NONE 300
#define RPL_AWAY 301
#define RPL_USERHOST 302
#define RPL_UNAWAY 305
#define RPL_NOWAWAY 306
#define RPL_WHOISREGNICK 307
#define RPL_WHOISUSER 311
#define RPL_WHOISSERVER 312
#define RPL_WHOISOPERATOR 313
#define RPL_WHOWASUSER 314
#define RPL_ENDOFWHO 315
#define RPL_WHOISIDLE 317
#define RPL_ENDOFWHOIS 318
#define RPL_WHOISCHANNELS 319
#define RPL_WHOISSPECIAL 320
#define RPL_LISTSTART 321
#define RPL_LIST 322
#define RPL_LISTEND 323
#define RPL_CHANNELMODEIS_NUM 324
#define RPL_CREATIONTIME 329
#define RPL_WHOISACCOUNT 330
#define RPL_NOTOPIC_NUM 331
#define RPL_TOPIC_NUM 332
#define RPL_TOPICWHOTIME_NUM 333
#define RPL_INVITELIST 336
#define RPL_ENDOFINVITELIST 337
#define RPL_WHOISACTUALLY 338
#define RPL_INVITING 341
#define RPL_INVEXLIST 346
#define RPL_ENDOFINVEXLIST 347
#define RPL_EXCEPTLIST 348
#define RPL_ENDOFEXCEPTLIST 349
#define RPL_VERSION 351
#define RPL_WHOREPLY 352
// #define RPL_NAMREPLY 353
#define RPL_LINKS 364
#define RPL_ENDOFLINKS 365
// #define RPL_ENDOFNAMES 366
#define RPL_BANLIST 367
#define RPL_ENDOFBANLIST 368
#define RPL_ENDOFWHOWAS 369
#define RPL_INFO 371
#define RPL_MOTD 372
#define RPL_ENDOFINFO 374
#define RPL_MOTDSTART 375
#define RPL_ENDOFMOTD 376
#define RPL_WHOISHOST 378
#define RPL_WHOISMODES 379
#define RPL_YOUREOPER 381
#define RPL_REHASHING 382
#define RPL_TIME 391
#define ERR_UNKNOWNERROR 400
#define ERR_NOSUCHNICK_NUM 401
#define ERR_NOSUCHSERVER 402
#define ERR_NOSUCHCHANNEL_NUM 403
// #define ERR_CANNOTSENDTOCHAN 404
// #define ERR_TOOMANYCHANNELS 405
#define ERR_WASNOSUCHNICK 406
#define ERR_NOORIGIN 409
// #define ERR_NORECIPIENT 411
// #define ERR_NOTEXTTOSEND 412
#define ERR_INPUTTOOLONG 417
#define ERR_UNKNOWNCOMMAND_NUM 421
#define ERR_NOMOTD 422
#define ERR_NONICKNAMEGIVEN 431
#define ERR_ERRONEUSNICKNAME 432
#define ERR_NICKNAMEINUSE 433
#define ERR_NICKCOLLISION 436
#define ERR_USERNOTINCHANNEL 441
#define ERR_NOTONCHANNEL_NUM 442
#define ERR_USERONCHANNEL 443
#define ERR_NOTREGISTERED_NUM 451
#define ERR_NEEDMOREPARAMS_NUM 461
#define ERR_ALREADYREGISTERED 462
#define ERR_PASSWDMISMATCH 464
#define ERR_YOUREBANNEDCREEP 465
#define ERR_CHANNELISFULL_NUM 471
#define ERR_UNKNOWNMODE_NUM 472
#define ERR_INVITEONLYCHAN_NUM 473
#define ERR_BANNEDFROMCHAN 474
#define ERR_BADCHANNELKEY_NUM 475
#define ERR_BADCHANMASK_NUM 476
#define ERR_NOPRIVILEGES 481
#define ERR_CHANOPRIVSNEEDED_NUM 482
#define ERR_CANTKILLSERVER 483
#define ERR_NOOPERHOST 491
#define ERR_UMODEUNKNOWNFLAG 501
#define ERR_USERSDONTMATCH 502
#define ERR_HELPNOTFOUND 524
#define ERR_INVALIDKEY 525
#define RPL_STARTTLS 670
#define RPL_WHOISSECURE 671
#define ERR_STARTTLS 691
#define ERR_INVALIDMODEPARAM_NUM 696
#define RPL_HELPSTART 704
#define RPL_HELPTXT 705
#define RPL_ENDOFHELP 706
#define ERR_NOPRIVS 723
#define RPL_LOGGEDIN 900
#define RPL_LOGGEDOUT 901
#define ERR_NICKLOCKED 902
#define RPL_SASLSUCCESS 903
#define ERR_SASLFAIL 904
#define ERR_SASLTOOLONG 905
#define ERR_SASLABORTED 906
#define ERR_SASLALREADY 907
#define RPL_SASLMECHS 908

// -------command replise list--------

// ------공통------
#define ERR_NOSUCHCHANNEL(nickname, channel) (":irc.local 403 " + nickname + " " + channel + " :No such channel\r\n")
#define ERR_NEEDMOREPARAMS(nickname, command) (":irc.local 461 " + nickname + " " + command + " :Not enough parameters\r\n")
#define ERR_NOSUCHNICK(nickname, target) (":irc.local 401 " + nickname + " " + target + " :No such nick\r\n")
#define ERR_CHANOPRIVSNEEDED(channel) (":irc.local 482 " + channel + " :You're not channel operator\r\n")
#define ERR_UNKNOWNCOMMAND(nickname, command) (":irc.local 421 " + nickname + " " + command + " :Unknown command\r\n")
#define ERR_NOTREGISTERED(command) (":irc.local 451 * " + command + " :You have not registered.\r\n")

// ------WELCOME-------
#define RPL_WELCOME(nickname, client) (":irc.local 001 " + nickname + " :Welcome to the TEST IRC Network " + client + "\r\n")


// -----------JOIN------------
//#define JOIN_REPLISE(nickname, username, channel) (":" + nickname + "!" + username + "@localhost JOIN :" + channel + "\r\n")
#define JOIN_REPLISE(client, channel) (":" + client + " JOIN :" + channel + "\r\n")
#define KICK_REPLY(nickname, username, channel, toKick, reason) (":" + nickname + "!" + username + "@localhost KICK " + channel + " " + toKick + " " + reason + "\r\n")
// #define RPL_NAMREPLY(nickname, nickname_list, channel) (":irc.local 353 " + nickname + " @ " + channel + " :" + nickname_list + "\r\n")
#define RPL_NAMREPLY(nickname, channel) (":irc.local 353 " + nickname + " = " + channel + " :@")
#define RPL_ENDOFNAMES(nickname, channel) (":irc.local 366 " + nickname + " " + channel + " :End of /NAMES list.\r\n")
#define RPL_TOPIC(channel, nickname, topic) (":irc.local 332 " + nickname + " " + channel + " " + topic + "\r\n")
#define ERR_TOOMANYCHANNELS(channel, nickname) (":irc.local 405 " +nickname + " " + channel + " :You have joined too many channels\r\n")
#define ERR_CHANNELISFULL(channel, nickname) (":irc.local 471 " + nickname + " " + channel + " :Cannot join channel (+l)\r\n")
#define ERR_BADCHANNELKEY(nickname, channel) (":irc.local 475 " + nickname + " " + channel + " :Cannot join channel (+k)\r\n")
#define ERR_INVITEONLYCHAN(channel, nickname) (":irc.local 473 " + nickname + " " + channel + " :Cannot join channel (+i)\r\n")
#define ERR_BADCHANMASK(channel) (":irc.local 476 " + channel + " :Bad Channel Mask\r\n")

// ----------PRIVMSG----------
#define PRIVMSG_REPLISE(client, target, response) (":" + client + " PRIVMSG " + target + " :" + response + "\r\n")
//#define ERR_NOSUCHNICK(nickname) (":irc.local 401 " + nickname + " :No such nick/channel\r\n")
#define ERR_NOTEXTTOSEND(nickname) (":irc.local 412 " + nickname + " :No text to send\r\n")
#define ERR_NORECIPIENT(nickname) (":irc.local 411 " + nickname + " :No recipient given PRIVMSG\r\n")
#define ERR_CANNOTSENDTOCHAN(nickname, channel) (":irc.local 404 " + nickname + " " + channel + " :Cannot send to channel\r\n")

// ---------PART------------
#define PART_REPLISE(client, channel) (":" + client + " PART :" + channel + "\r\n")
#define PART_REASON_REPLISE(client, channel, reason) (":" + client + " PART " + channel + " :" + reason + "\r\n")
#define ERR_NOTONCHANNEL(channel, nickname) (":irc.local 442 " + nickname + " " + channel + " :You're not on that channel\r\n")

// ----------MODE-----------
#define RPL_CHANNELMODEIS(nickname, channel, mode) (":irc.local 324 " + nickname + " " + channel + " :" + mode + "\r\n")
#define ERR_UNKNOWNMODE(nickname, flag) (":irc.local 472 " + nickname + " " + flag + " :is not a recognised channel mode.\r\n")
// 일단 대충 지음
#define ERR_INVALIDMODEPARAM(nickname, channel, mode, description) (":irc.local 696 " + nickname + " " + channel + " " + mode + " * :" + description + "\r\n")
#define ERR_KEYMISSMATCH(nickname, channel) (":irc.local 467 " + nickname + " " + channel + " :Channel key already set\r\n")
#define RPL_MODEREPLIES(client, channel, result) (":" + client + " MODE " + channel + " " + result + "\r\n")


// ----------TOPIC-----------
#define RPL_NOTOPIC(channel, nickname) (":irc.local 331 " + nickname + " " + channel + " :No topic is set\r\n")

#endif